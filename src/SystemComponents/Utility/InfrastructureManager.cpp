//
// Created by Bernardo on 4/4/2018.
//

#include <algorithm>
#include "InfrastructureManager.h"
#include "../WaterSources/ReservoirExpansion.h"
#include "../WaterSources/Relocation.h"
#include "../../Utils/Utils.h"
#include "../WaterSources/JointTreatmentCapacityExpansion.h"

InfrastructureManager::InfrastructureManager(
        int id, const vector<double> &infra_construction_triggers,
        const vector<vector<int>> &infra_if_built_remove,
        double infra_discount_rate, vector<int> rof_infra_construction_order,
        vector<int> demand_infra_construction_order)
        :
        id(id),
        infra_construction_triggers(rearrangeInfraRofVector
                                            (infra_construction_triggers,
                                             rof_infra_construction_order,
                                             demand_infra_construction_order)),
        infra_discount_rate(infra_discount_rate),
        rof_infra_construction_order(rof_infra_construction_order),
        demand_infra_construction_order(demand_infra_construction_order) {


    rof_infra_construction_order.insert(rof_infra_construction_order.end(),
                                        demand_infra_construction_order.begin(),
                                        demand_infra_construction_order.end());
    auto size = *max_element(rof_infra_construction_order.begin(),
                             rof_infra_construction_order.end()) + 1;

    restructureVectorOfRelatedInfrastructure(this->infra_if_built_remove,
                                             infra_if_built_remove, size);
}

void InfrastructureManager::restructureVectorOfRelatedInfrastructure(
        vector<vector<int>> &vec, const vector<vector<int>> &vec_original,
        int size) {

    vec.resize(size);
    for (const auto &v : vec_original) {
        for (unsigned long ws = 1; ws < v.size(); ++ws) {
            vec.at(v.at(ws)) = vector<int>(v.begin(), v.begin() + ws);
        }
    }
}


InfrastructureManager::InfrastructureManager(
        InfrastructureManager &infrastructure_manager) :
        id(infrastructure_manager.id),
        infra_construction_triggers(
                infrastructure_manager.infra_construction_triggers),
        infra_if_built_remove(infrastructure_manager.infra_if_built_remove),
        infra_discount_rate(infrastructure_manager.infra_discount_rate),
        rof_infra_construction_order(
                infrastructure_manager.rof_infra_construction_order),
        demand_infra_construction_order(
                infrastructure_manager.demand_infra_construction_order) {}

InfrastructureManager::InfrastructureManager() = default;

InfrastructureManager &InfrastructureManager::operator=(
        const InfrastructureManager &infrastructure_manager) {
    id = infrastructure_manager.id;
    infra_construction_triggers = infrastructure_manager.infra_construction_triggers;
    infra_if_built_remove = infrastructure_manager.infra_if_built_remove;
    infra_discount_rate = infrastructure_manager.infra_discount_rate;
    rof_infra_construction_order = infrastructure_manager.rof_infra_construction_order;
    demand_infra_construction_order = infrastructure_manager.demand_infra_construction_order;

    return *this;
}

/**
 * Connects a reservoir to the utility.
 * @param water_source
 */
void InfrastructureManager::addWaterSource(WaterSource *water_source) {
    // Add water sources with their IDs matching the water sources vector
    // indexes.
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
 * @return vector with construction triggers on positions corresponding to ids
 * of pertinent sources and 1e10 o/w.
 */
vector<double> InfrastructureManager::rearrangeInfraRofVector(
        const vector<double> &infra_construction_triggers,
        const vector<int> &rof_infra_construction_order,
        const vector<int> &demand_infra_construction_order) {
    int size_rof = (rof_infra_construction_order.empty() ? 0 :
                    *max_element(rof_infra_construction_order.begin(),
                                 rof_infra_construction_order.end()));
    int size_demand = (demand_infra_construction_order.empty() ? 0 :
                       *max_element(demand_infra_construction_order.begin(),
                                    demand_infra_construction_order.end()));
    int size = max(size_rof, size_demand) + 1;

    auto n_options = max(rof_infra_construction_order.size(),
                         demand_infra_construction_order.size());
    if (infra_construction_triggers.size() != n_options) {
        char error[200];
        sprintf(error,
                "Number of ROF or demand triggers (%lu) for utility %d must match the number of "
                "infrastructure options triggered by ROf or demand (%lu, %lu, respectively).",
                infra_construction_triggers.size(), id,
                rof_infra_construction_order.size(),
                demand_infra_construction_order.size());
        throw invalid_argument(error);
    }

    vector<double> infra_construction_triggers_new((unsigned long) size, 1e10);
    for (unsigned long i = 0; i < rof_infra_construction_order.size(); ++i) {
        auto ws = (unsigned long) rof_infra_construction_order.at(i);
        infra_construction_triggers_new.at(ws) = infra_construction_triggers.at(
                i);
    }

    for (unsigned long i = 0; i < demand_infra_construction_order.size(); ++i) {
        auto ws = (unsigned long) demand_infra_construction_order.at(i);
        if (infra_construction_triggers_new.at(ws) != 1e10)
            throw invalid_argument("A source can be triggered only by "
                                   "either rof or by demand.");
        infra_construction_triggers_new.at(
                (unsigned long) demand_infra_construction_order.at(i)) =
                infra_construction_triggers.at(i);
    }

    return infra_construction_triggers_new;
}

void InfrastructureManager::addWaterSourceToOnlineLists(int source_id,
                                                        double &total_storage_capacity,
                                                        double &total_treatment_capacity,
                                                        double &total_available_volume,
                                                        double &total_stored_volume) {
    auto ws = water_sources->at((unsigned long) source_id);

    // Add capacities to utility.
    total_storage_capacity +=
            ws->getAllocatedCapacity(id);
    total_treatment_capacity +=
            ws->getAllocatedTreatmentCapacity(id);
    total_available_volume = total_storage_capacity;

    // Add source to the corresponding list of online water sources.
    if ((ws->source_type == INTAKE ||
         ws->source_type == WATER_REUSE)) {
        priority_draw_water_source->push_back(source_id);
    } else {
        non_priority_draw_water_source->push_back(source_id);
        total_stored_volume += ws->getAvailableAllocatedVolume(id);
    }
}


void
InfrastructureManager::setWaterSourceOnline(unsigned int source_id, int week,
                                            double &total_storage_capacity,
                                            double &total_treatment_capacity,
                                            double &total_available_volume,
                                            double &total_stored_volume) {
    // Sets water source online and add its ID to appropriate
    // priority/non-priority ID vector. If reservoir expansion, add its
    // capacity to the corresponding existing reservoir.
    if (water_sources->at(source_id)->source_type ==
        NEW_SEQUENTIAL_WATER_TREATMENT_PLANT) {
        waterTreatmentPlantConstructionHandler(source_id,
                                               total_storage_capacity);
    } else if (water_sources->at(source_id)->source_type ==
               NEW_JOINT_WATER_TREATMENT_PLANT) {
        waterTreatmentPlantConstructionHandler(source_id,
                                               total_storage_capacity);
    } else if (water_sources->at(source_id)->source_type ==
               RESERVOIR_EXPANSION) {
        reservoirExpansionConstructionHandler(source_id);
    } else if (water_sources->at(source_id)->source_type == SOURCE_RELOCATION) {
        sourceRelocationConstructionHandler(source_id);
    } else {
        water_sources->at(source_id)->setOnline();
        addWaterSourceToOnlineLists(source_id, total_storage_capacity,
                                    total_treatment_capacity,
                                    total_available_volume,
                                    total_stored_volume);
    }

    // Updates total storage and treatment variables.
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

void InfrastructureManager::waterTreatmentPlantConstructionHandler(
        unsigned int source_id, double &total_storage_capacity) {
    auto wtp = dynamic_cast<JointTreatmentCapacityExpansion *>(water_sources->at(
            source_id));

    // Add treatment capacity to source
    try {
        // added_treatment_capacities is a map in which .first is the id and
        // .second is the capacity
        water_sources->at(wtp->parent_source_id)
                ->addTreatmentCapacity(
                        wtp->added_treatment_capacities.at(id), id);

        // If source is an existing intake or reuse and is not in the utility's list
        // of active sources, add it to the priority list.
        // If source is active, is not and intake or reuse, and is not in the
        // utility's list of active sources, add it to the non-priority list.
        bool is_priority_source =
                water_sources->at(wtp->parent_source_id)->source_type ==
                INTAKE ||
                water_sources->at(wtp->parent_source_id)->source_type ==
                WATER_REUSE;
        bool is_not_in_priority_list =
                find(priority_draw_water_source->begin(),
                     priority_draw_water_source->end(),
                     wtp->parent_source_id)
                == priority_draw_water_source->end();
        bool is_not_in_non_priority_list =
                find(non_priority_draw_water_source->begin(),
                     non_priority_draw_water_source->end(),
                     wtp->parent_source_id)
                == non_priority_draw_water_source->end();

        // Finally, the checking.
        if (is_priority_source && is_not_in_priority_list) {
            priority_draw_water_source->push_back(wtp->parent_source_id);
            total_storage_capacity +=
                    water_sources->at(wtp->parent_source_id)
                            ->getAllocatedCapacity(id);
        } else if (is_not_in_non_priority_list) {
            non_priority_draw_water_source
                    ->push_back(wtp->parent_source_id);
        }
    } catch (...) {
        throw runtime_error("Could not add treatment capacity to "
                            "infrastructure.");
    }

    water_sources->at(source_id)->setOnline();
}

void InfrastructureManager::reservoirExpansionConstructionHandler(
        unsigned int source_id) {
    ReservoirExpansion re =
            *dynamic_cast<ReservoirExpansion *>(water_sources->at(source_id));

    water_sources->at(re.parent_reservoir_ID)->
            addCapacity(re.getAllocatedCapacity(id));
    water_sources->at(source_id)->setOnline();
}

void InfrastructureManager::sourceRelocationConstructionHandler(
        unsigned int source_id) {
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
        // Variable storing the position of an infrastructure option in the
        // queue of infrastructure to be built.
        auto it = find(rof_infra_construction_order.begin(),
                       rof_infra_construction_order.end(),
                       ws);
        // If one of the infrastructure options built this year by one of
        // the other utilities is in this utility's queue, force it to be
        // triggered.
        if (it != rof_infra_construction_order.end()) {
            // Force infrastructure option new_infra_triggered to be built.
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
int
InfrastructureManager::infrastructureConstructionHandler(double long_term_rof,
                                                         int week,
                                                         double past_year_average_demand,
                                                         double &total_storage_capacity,
                                                         double &total_treatment_capacity,
                                                         double &total_available_volume,
                                                         double &total_stored_volume) {
    int new_infra_triggered = NON_INITIALIZED;
    bool under_construction_any = (find(under_construction.begin(),
                                        under_construction.end(), true) !=
                                   under_construction.end());


    if (demand_infra_construction_order.empty()) {
        new_infra_triggered = getIdOfNewTriggeredInfra(
                long_term_rof, week, under_construction_any,
                rof_infra_construction_order);
    } else {
        new_infra_triggered = getIdOfNewTriggeredInfra(
                past_year_average_demand, week, under_construction_any,
                demand_infra_construction_order);
    }

    // If there's a water source under construction, check if it's ready and,
    // if so, clear it from the to-build list.
    if (under_construction_any) {
        // Loops through vector of under-construction flags and sets online
        // the ones for which construction period has passed.
        for (unsigned long ws = 0; ws < under_construction.size(); ++ws)
            if (under_construction.at(ws) &&
                week > construction_end_date.at(ws)) {
                vector<int> set_online_now;

                // Set online all sources that to be set online now.
                setWaterSourceOnline((unsigned int) ws, week,
                                     total_storage_capacity,
                                     total_treatment_capacity,
                                     total_available_volume,
                                     total_stored_volume);

                // Record ID of and when infrastructure option construction was
                // completed. (utility_id, week, new source id)
                infra_built_last_week = {id, week, (int) ws};

                // Erase infrastructure option from both vectors of
                // infrastructure to be built.
                if (!rof_infra_construction_order.empty())
                    Utils::removeIntFromVector(rof_infra_construction_order,
                                               ws);

                else if (!demand_infra_construction_order.empty())
                    Utils::removeIntFromVector(
                            demand_infra_construction_order, ws);
                else
                    throw logic_error(
                            "Infrastructure option whose construction was"
                            " complete is not in the demand or "
                            "rof triggered construction lists.");

                under_construction[ws] = false;

            }
    }

    return new_infra_triggered;
}

int
InfrastructureManager::getIdOfNewTriggeredInfra(double trigger_var, int week,
                                                bool under_construction_any,
                                                const vector<int> &construction_order) {
    int infra_triggered = NON_INITIALIZED;

    // Checks whether there are still options in the demand and long-term ROF
    // queues, if there's nothing already under construction, and if there's
    // nothing on triggered queue. If all three conditions are met, check
    // permitting periods and trigger metrics before starting construction.
    if (!construction_order.empty() && !under_construction_any &&
        triggered_queue.empty()) {

        vector<int> construction_prerequisites;
        // Selects next water source whose permitting period is passed.
        int next_construction = NON_INITIALIZED;
        for (int id : construction_order) {
            auto pp = water_sources->at(
                    id)->getPermitting_period(); // permitting period of triggered source

            // if option requiring other to be built together (e.g. second stage
            // of a project normally requires first stage complete) is triggered,
            // use the maximum permitting period between all dependent projects.
            construction_prerequisites = water_sources->at(
                    id)->getConstructionPrerequisites();
            try {
                for (int id_also : construction_prerequisites) {
                    pp = max(pp,
                             water_sources->at(
                                     id_also)->getPermitting_period());
                }
            } catch (out_of_range &e) {}

            // Check for permitting period
            if (week > pp) {
                next_construction = id;
                break;
            }
        }

        // Checks if value of trigger metric for next infrastructure in line has
        // been reached and if there is already infrastructure being built.
        if (next_construction != NON_INITIALIZED &&
            trigger_var > infra_construction_triggers.at(next_construction)) {
            if (!construction_prerequisites.empty() &&
                !construction_prerequisites.empty()) {
                triggered_queue = construction_prerequisites;
            }
            triggered_queue.push_back(next_construction);

            beginConstruction(week, triggered_queue[0]);
            infra_triggered = triggered_queue[0];
            triggered_queue.erase(triggered_queue.begin());
        }
        // If there's nothing under construction but there's an option on the queue,
        // start building it next in queue.
    } else if (!under_construction_any && !triggered_queue.empty()) {
        beginConstruction(week, triggered_queue[0]);
        infra_triggered = triggered_queue[0];
        triggered_queue.erase(triggered_queue.begin());
    }

    return infra_triggered;
}

/**
 * Checks if piece of infrastructure to be built next prevents another one
 * from being build and, if so, removes the latter from the queue.
 * @param next_construction
 */
void
InfrastructureManager::removeRelatedSourcesFromQueue(int next_construction) {
    for (auto &v : infra_if_built_remove) {
        if (!v.empty() && v[0] == next_construction) {
            for (int i : v) {
                Utils::removeIntFromVector(rof_infra_construction_order, i);
                Utils::removeIntFromVector(demand_infra_construction_order,
                                           i);
            }
        }
    }

}


/**
 * Kicks off construction and records when it was initiated.
 * @param week
 */
void InfrastructureManager::beginConstruction(int week, int infra_id) {
    // Checks is piece of infrastructure to be built next prevents
    // another one from being build and, if so, removes the latter
    // from the queue. E.g. if the large expansion of a reservoir is
    // triggered the small expansion must be removed from the to
    // build list.
    removeRelatedSourcesFromQueue(infra_id);

    // Make water utility as under construction and determine construction
    // end date (I wish that was how it worked in real constructions...)
    try {
        under_construction[infra_id] = true;
        construction_end_date[infra_id] =
                week + (int) water_sources->at(
                        (unsigned long) infra_id)->construction_time;
    } catch (...) {
        throw out_of_range(
                "Infrastructure not present in infrastructure manager (in utility).");
    }
}

void InfrastructureManager::connectWaterSourcesVectorsToUtilitys(
        vector<WaterSource *> &water_sources,
        vector<int> &priority_draw_water_source,
        vector<int> &non_priority_draw_water_source) {
    this->water_sources = &water_sources;
    this->priority_draw_water_source = &priority_draw_water_source;
    this->non_priority_draw_water_source = &non_priority_draw_water_source;
}

const vector<int> &
InfrastructureManager::getRof_infra_construction_order() const {
    return rof_infra_construction_order;
}

const vector<int> &
InfrastructureManager::getDemand_infra_construction_order() const {
    return demand_infra_construction_order;
}

const vector<int> &InfrastructureManager::getInfra_built_last_week() const {
    return infra_built_last_week;
}

const vector<bool> &InfrastructureManager::getUnder_construction() const {
    return under_construction;
}

const vector<double> &
InfrastructureManager::getInfraConstructionTriggers() const {
    return infra_construction_triggers;
}

int InfrastructureManager::getId() const {
    return id;
}

double InfrastructureManager::getInfraDiscountRate() const {
    return infra_discount_rate;
}

