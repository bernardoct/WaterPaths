//
// Created by Bernardo on 4/4/2018.
//

#include <algorithm>
#include "InfrastructureManager.h"
#include "../WaterSources/SequentialJointTreatmentExpansion.h"
#include "../WaterSources/ReservoirExpansion.h"
#include "../WaterSources/Relocation.h"
#include "../../Utils/Utils.h"

InfrastructureManager::InfrastructureManager(int id, const vector<double> &infra_construction_triggers,
                                             const vector<vector<int>> &infra_if_built_remove,
                                             double infra_discount_rate, double bond_term,
                                             double bond_interest_rate, vector<int> rof_infra_construction_order,
                                             vector<int> demand_infra_construction_order) :
        id(id),
        infra_construction_triggers(rearrangeInfraRofVector
                                            (infra_construction_triggers, rof_infra_construction_order,
                                             demand_infra_construction_order)),
        infra_if_built_remove(infra_if_built_remove),
        infra_discount_rate(infra_discount_rate),
        bond_term(bond_term),
        bond_interest_rate(bond_interest_rate),
        rof_infra_construction_order(rof_infra_construction_order),
        demand_infra_construction_order(demand_infra_construction_order) {}


InfrastructureManager::InfrastructureManager() {}


InfrastructureManager::InfrastructureManager(InfrastructureManager &infrastructure_manager) :
        id(infrastructure_manager.id),
        infra_construction_triggers(infrastructure_manager.infra_construction_triggers),
        infra_if_built_remove(infrastructure_manager.infra_if_built_remove),
        infra_discount_rate(infrastructure_manager.infra_discount_rate),
        bond_term(infrastructure_manager.bond_term),
        bond_interest_rate(infrastructure_manager.bond_interest_rate),
        rof_infra_construction_order(infrastructure_manager.rof_infra_construction_order),
        demand_infra_construction_order(infrastructure_manager.demand_infra_construction_order) {}

InfrastructureManager& InfrastructureManager::operator=(const InfrastructureManager& infrastructure_manager) {
    id = infrastructure_manager.id;
    infra_construction_triggers = infrastructure_manager.infra_construction_triggers;
    infra_if_built_remove = infrastructure_manager.infra_if_built_remove;
    infra_discount_rate = infrastructure_manager.infra_discount_rate;
    bond_term = infrastructure_manager.bond_term;
    bond_interest_rate = infrastructure_manager.bond_interest_rate;
    rof_infra_construction_order = infrastructure_manager.rof_infra_construction_order;
    demand_infra_construction_order = infrastructure_manager.demand_infra_construction_order;

    return *this;
}

/**
 * Connects a reservoir to the utility.
 * @param water_source
 */
void InfrastructureManager::addWaterSource(WaterSource *water_source) {
    /// Add water sources with their IDs matching the water sources vector
    /// indexes.
    if (water_source->id > (int) under_construction.size() - 1) {
        under_construction.resize(water_sources->size(), false);
        construction_end_date.resize(water_sources->size(), NON_INITIALIZED);
    }
}

/**
 * Converts the rof and demand triggers vectors into a vector in which rof
 * and demand values are stored in the idex corresponding to the
 * pertaining source.
 * @param infra_construction_triggers
 * @param rof_infra_construction_order
 * @param demand_infra_construction_order
 * @return
 */
vector<double> InfrastructureManager::rearrangeInfraRofVector(
        const vector<double>& infra_construction_triggers,
        const vector<int>& rof_infra_construction_order,
        const vector<int>& demand_infra_construction_order) {
    int size_rof = (rof_infra_construction_order.empty() ? 0 :
                    *max_element(rof_infra_construction_order.begin(),
                                 rof_infra_construction_order.end()));
    int size_demand = (demand_infra_construction_order.empty() ? 0 :
                       *max_element(demand_infra_construction_order.begin(),
                                    demand_infra_construction_order.end()));
    int size = max(size_rof, size_demand) + 1;

    vector<double> infra_construction_triggers_new((unsigned long) size, 1e10);
    for (int i = 0; i < (int) rof_infra_construction_order.size(); ++i) {
        int ws = rof_infra_construction_order[i];
        infra_construction_triggers_new[ws] = infra_construction_triggers[i];
    }

    for (int i = 0; i < (int) demand_infra_construction_order.size(); ++i) {
        int ws = demand_infra_construction_order[i];
        if (infra_construction_triggers_new[ws] != 1e10)
            throw invalid_argument("A source can be triggered only by "
                                     "either rof or by demand.");
        infra_construction_triggers_new[demand_infra_construction_order[i]] =
                infra_construction_triggers[i];
    }

    return infra_construction_triggers_new;
}

void InfrastructureManager::addWaterSourceToOnlineLists(int source_id, double &total_storage_capacity,
                                                        double &total_treatment_capacity,
                                                        double &total_available_volume,
                                                        double &total_stored_volume) {
    auto ws = water_sources->at((unsigned long) source_id);

    /// Add capacities to utility.
    total_storage_capacity +=
            ws->getAllocatedCapacity(id);
    total_treatment_capacity +=
            ws->getAllocatedTreatmentCapacity(id);
    total_available_volume = total_storage_capacity;

    /// Add source to the corresponding list of online water sources.
    if ((ws->source_type == INTAKE ||
         ws->source_type == WATER_REUSE)) {
        priority_draw_water_source->push_back(source_id);
    } else {
        non_priority_draw_water_source->push_back(source_id);
        total_stored_volume += ws->getAvailableAllocatedVolume(id);
    }
}


void
InfrastructureManager::setWaterSourceOnline(unsigned int source_id, int week, double &total_storage_capacity,
        double &total_treatment_capacity, double &total_available_volume, double &total_stored_volume) {
    /// Sets water source online and add its ID to appropriate
    /// priority/non-priority ID vector. If reservoir expansion, add its
    /// capacity to the corresponding existing reservoir.
    if (water_sources->at(source_id)->source_type == NEW_WATER_TREATMENT_PLANT) {
        waterTreatmentPlantConstructionHandler(source_id, total_storage_capacity);
    } else if (water_sources->at(source_id)->source_type ==
               RESERVOIR_EXPANSION) {
        reservoirExpansionConstructionHandler(source_id);
    } else if (water_sources->at(source_id)->source_type == SOURCE_RELOCATION) {
        sourceRelocationConstructionHandler(source_id);
    } else {
        water_sources->at(source_id)->setOnline();
        addWaterSourceToOnlineLists(source_id, total_storage_capacity, total_treatment_capacity,
                                    total_available_volume, total_stored_volume);
    }

    /// Updates total storage and treatment variables.
    total_storage_capacity = 0;
    total_treatment_capacity = 0;
    total_available_volume = 0;
    for (WaterSource *ws : *water_sources) {
        if (ws &&
            (find(priority_draw_water_source->begin(),
                  priority_draw_water_source->end(),
                  ws->id) != priority_draw_water_source->end() ||
             find(non_priority_draw_water_source->begin(),
                  non_priority_draw_water_source->end(),
                  ws->id) != non_priority_draw_water_source->end())) {
            total_storage_capacity += ws->getAllocatedCapacity(id);
            total_treatment_capacity += ws->getAllocatedTreatmentCapacity(id);
            total_available_volume += ws->getAvailableAllocatedVolume(id);
        }
    }
}


void
InfrastructureManager::waterTreatmentPlantConstructionHandler(unsigned int source_id, double &total_storage_capacity) {
    auto wtp = dynamic_cast<SequentialJointTreatmentExpansion *>
    (water_sources->at(source_id));

    /// Add treatment capacity to source
    double added_capacity = wtp->implementTreatmentCapacity(id);
    try {
        water_sources->at(wtp->parent_reservoir_ID)
                ->addTreatmentCapacity(added_capacity, id);
    } catch (...) {
        throw runtime_error("Could not add treatment capacity to reservoir.");
    }

    /// If source is intake or reuse and is not in the list of active
    /// sources, add it to the priority list.
    /// If source is not intake or reuse and is not in the list of active
    /// sources, add it to the non-priority list.
    bool is_priority_source =
            water_sources->at(wtp->parent_reservoir_ID)->source_type == INTAKE ||
            water_sources->at(wtp->parent_reservoir_ID)->source_type ==
            WATER_REUSE;
    bool is_not_in_priority_list =
            find(priority_draw_water_source->begin(),
                 priority_draw_water_source->end(),
                 wtp->parent_reservoir_ID)
            == priority_draw_water_source->end();
    bool is_not_in_non_priority_list =
            find(non_priority_draw_water_source->begin(),
                 non_priority_draw_water_source->end(),
                 wtp->parent_reservoir_ID)
            == non_priority_draw_water_source->end();

    /// Finally, the checking.
    if (is_priority_source && is_not_in_priority_list) {
        priority_draw_water_source->push_back((int) wtp->parent_reservoir_ID);
        total_storage_capacity +=
                water_sources->at(wtp->parent_reservoir_ID)
                        ->getAllocatedCapacity(id);
    } else if (is_not_in_non_priority_list) {
        non_priority_draw_water_source
                ->push_back((int) wtp->parent_reservoir_ID);
    }
    water_sources->at(source_id)->setOnline();
}

void InfrastructureManager::reservoirExpansionConstructionHandler(unsigned int source_id) {
    ReservoirExpansion re =
            *dynamic_cast<ReservoirExpansion *>(water_sources->at(source_id));

    water_sources->at(re.parent_reservoir_ID)->
            addCapacity(re.getAllocatedCapacity(id));
    water_sources->at(source_id)->setOnline();
}

void InfrastructureManager::sourceRelocationConstructionHandler(unsigned int source_id) {
    Relocation re =
            *dynamic_cast<Relocation *>(water_sources->at(source_id));

    const vector<double> *new_allocated_fractions = re.new_allocated_fractions;

    water_sources->at(re.parent_reservoir_ID)->
            resetAllocations(new_allocated_fractions);
    water_sources->at(source_id)->setOnline();
}



/**
 * Force utility to build a certain infrastructure option, if present in its
 * queue.
 * @param week
 * @param new_infra_triggered
 */
void InfrastructureManager::forceInfrastructureConstruction(
        int week, vector<int> new_infra_triggered) {
    for (int ws : new_infra_triggered) {
        /// Variable storing the position of an infrastructure option in the
        /// queue of infrastructure to be built.
        auto it = find(rof_infra_construction_order.begin(),
                       rof_infra_construction_order.end(),
                       ws);
        /// If one of the infrastructure options built this year by one of
        /// the other utilities is in this utility's queue, force it to be
        /// triggered.
        if (it != rof_infra_construction_order.end()) {
            /// Force infrastructure option new_infra_triggered to be built.
            beginConstruction(week, ws);
        }
    }
}

/**
 * Checks if infrastructure should be built, triggers construction, sets it
 * online, and handles accounting/financing of it.
 * @param long_term_rof
 * @param week
 */
int InfrastructureManager::infrastructureConstructionHandler(double long_term_rof, int week, double past_year_average_demand,
                                                             double &total_storage_capacity, double &total_treatment_capacity,
                                                             double &total_available_volume, double &total_stored_volume) {
    int new_infra_triggered = NON_INITIALIZED;
    bool under_construction_any = (find(under_construction.begin(),
                                        under_construction.end(), true) !=
                                   under_construction.end());

    /// Checks whether the long-term ROF has been exceeded for the next
    /// infrastructure option in the list and, if not already under
    /// construction, starts building it.
    if (!rof_infra_construction_order.empty() && !under_construction_any) {
        // if there is anything to be built

        /// Selects next water source whose permitting period is passed.
        int next_construction = NON_INITIALIZED;
        for (int id : rof_infra_construction_order) {
            auto idd = (unsigned long) id;
            if (week > water_sources->at(idd)->getPermitting_period() && !water_sources->at(idd)->skipConstruction(id)) {
                next_construction = id;
                break;
            }
        }

        /// Checks if ROF threshold for next infrastructure in line has been
        /// reached and if there is already infrastructure being built.
        if (next_construction != NON_INITIALIZED &&
            long_term_rof > infra_construction_triggers[next_construction]) {
            new_infra_triggered = next_construction;

            /// Begin construction.
            beginConstruction(week, next_construction);
        }
    }

    /// Checks whether the target demand has been exceeded for the next
    /// infrastructure option in the list and, if not already under
    /// construction, starts building it.
    if (!demand_infra_construction_order.empty() && !under_construction_any &&
        week > WEEKS_IN_YEAR) { // if there is anything to be built


        /// Selects next water source whose permitting period is passed.
        int next_construction = NON_INITIALIZED;
        for (int &id : demand_infra_construction_order) {
            auto idd = (unsigned long) id;
            if (week > water_sources->at(idd)->getPermitting_period() && !water_sources->at(idd)->skipConstruction(id)) {
                next_construction = id;
                break;
            }
        }
        /// Checks if demand threshold for next infrastructure in line has been
        /// reached and if there is already infrastructure being built.
        if (next_construction != NON_INITIALIZED &&
                past_year_average_demand >infra_construction_triggers[next_construction]) {
            new_infra_triggered = next_construction;

            /// Begin construction.
            beginConstruction(week, next_construction);
        }
    }

    /// If there's a water source under construction, check if it's ready and,
    /// if so, clear it from the to-build list.
    if (under_construction_any) {
        /// Loops through vector of under-construction flags and sets online
        /// the ones for which construction period has passed.
        for (unsigned long ws = 0; ws < under_construction.size(); ++ws)
            if (under_construction[ws] && week > construction_end_date[ws]) {
                vector<int> set_online_now;

                /// Check if water source to be set online is part of a series of expansions or constructions
                /// That depend on previous water sources being built, in which case it's as if all of them had
                /// been constructed at the same time. The total cost of building all at the same time is approximated
                /// by the summation of their individual costs, and the construction time is approximated as that of the
                /// source that was called -- both can be pretty rough approximations at times, so any improvement here
                /// is welcome.
                if (water_sources->at(ws)->getBuilt_in_sequence().empty()) {
                    set_online_now.push_back((int) ws);
                } else {
                    for (const int &id_build : water_sources->at(ws)->getBuilt_in_sequence()) {
                        /// Check if previous source/expansion is yet to be built.
                        bool yet_to_be_built = find(rof_infra_construction_order.begin(),
                                                    rof_infra_construction_order.end(), id_build)
                                               != rof_infra_construction_order.end() ||
                                               find(demand_infra_construction_order.begin(),
                                                    demand_infra_construction_order.end(), id_build)
                                               != demand_infra_construction_order.end();
                        if (yet_to_be_built)
                            set_online_now.push_back(id_build);
                        if (id_build == ws)
                            break;
                    }
                }

                /// Set online all sources that at to be set online now.
                for (const int &wss : set_online_now) {
                    setWaterSourceOnline((unsigned int) wss, week, total_storage_capacity, total_treatment_capacity,
                                         total_available_volume, total_stored_volume);

                    /// Record ID of and when infrastructure option construction was
                    /// completed. (utility_id, week, new source id)
                    infra_built_last_week = {id, week, wss};

                    /// Erase infrastructure option from both vectors of
                    /// infrastructure to be built.
                    if (!rof_infra_construction_order.empty())
                        Utils::removeIntFromVector(rof_infra_construction_order, wss);

                    else if (!demand_infra_construction_order.empty())
                        Utils::removeIntFromVector(demand_infra_construction_order, wss);
                    else
                        throw logic_error("Infrastructure option whose construction was"
                                            " complete is not in the demand or "
                                            "rof triggered construction lists.");

                    under_construction[wss] = false;
                }
            }
    }

    return new_infra_triggered;
}

/**
 * Checks is piece of infrastructure to be built next prevents another one
 * from being build and, if so, removes the latter from the queue.
 * @param next_construction
 */
void InfrastructureManager::removeRelatedSourcesFromQueue(int next_construction) {
    if (!infra_if_built_remove.empty()) {
        for (auto &v : infra_if_built_remove) {
            if (v[0] == next_construction) {
                for (int i : v) {
                    Utils::removeIntFromVector(rof_infra_construction_order, i);
                    Utils::removeIntFromVector(demand_infra_construction_order, i);
                }
            }
        }
    }
}



/**
 * Kicks off construction and records when it was initiated.
 * @param week
 */
void InfrastructureManager::beginConstruction(int week, int infra_id) {
    /// Checks is piece of infrastructure to be built next prevents
    /// another one from being build and, if so, removes the latter
    /// from the queue. E.g. if the large expansion of a reservoir is
    /// triggered the small expansion must be removed from the to
    /// build list.
    removeRelatedSourcesFromQueue(infra_id);

    /// Make water utility as under construction and determine construction
    /// end date (I wish that was how it worked in real constructions...)
    try {
        under_construction[infra_id] = true;
        construction_end_date[infra_id] =
                week + (int) water_sources->at((unsigned long) infra_id)->construction_time;
    } catch (...) {
        throw out_of_range("Infrastructure not present in infrastructure manager (in utility).");
    }
}

void InfrastructureManager::connectWaterSourcesVectorsToUtilitys(vector<WaterSource *> &water_sources,
                                                                 vector<int> &priority_draw_water_source,
                                                                 vector<int> &non_priority_draw_water_source) {
    this->water_sources = &water_sources;
    this->priority_draw_water_source = &priority_draw_water_source;
    this->non_priority_draw_water_source = &non_priority_draw_water_source;
}

const vector<int> &InfrastructureManager::getRof_infra_construction_order() const {
    return rof_infra_construction_order;
}

const vector<int> &InfrastructureManager::getDemand_infra_construction_order() const {
    return demand_infra_construction_order;
}

const vector<int> &InfrastructureManager::getInfra_built_last_week() const {
    return infra_built_last_week;
}

const vector<bool> &InfrastructureManager::getUnder_construction() const {
    return under_construction;
}

