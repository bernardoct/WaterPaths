#include <iostream>
#include <vector>
#include "Reservoir.h"
#include "Constants.h"
#include "Region.h"
#include "Aux.h"
#include "Catchment.h"


using namespace std;
using namespace Constants;

int reservoirTest() {
    cout << "BEGINNING RESERVOIR TEST" << endl << endl;

    Reservoir r(string("Lake Michie"), 8.4, 10.0, 0.5, ONLINE);

    cout << r.applyContinuity(4.0, 2.0) << endl;
    cout << r.getStored_volume() << endl;

    cout << endl << "END OF RESERVOIR TEST" << endl << "------------------------------------" << endl << endl;

    return 1;
}

int regionContinuityTest() {

    cout << "BEGINNING REGION CONTINUITY TEST" << endl << endl;

    int reservoirConnectivityMatrix[MAX_NUMBER_OF_RESERVOIRS][MAX_NUMBER_OF_RESERVOIRS] = {{}};
    reservoirConnectivityMatrix[0][1] = -1;
    reservoirConnectivityMatrix[1][0] = 1;

    vector<Reservoir> reservoirs;

    Reservoir *lake_michie = new Reservoir(string("Lake Michie"), 8.4, 10.0, 0.5, ONLINE);
    Reservoir *stone_quarry = new Reservoir(string("Stone Quarry"), 8.8, 10.0, 0.5, ONLINE);

    reservoirs.push_back(*lake_michie);
    reservoirs.push_back(*stone_quarry);

    Region region(reservoirs, reservoirConnectivityMatrix, 1);

    cout << endl << "END OF REGION CONTINUITY TEST" << endl << endl << endl;

    return 0;
}

int testReadCsv() {

    cout << "BEGINNING CSV READ TEST" << endl << endl;

    double **test_data;
    char *file_name = "/home/bernardo/ClionProjects/TriangleModel/TestFiles/updatedOWASAInflowSYN9.csv";
    test_data = Aux::parse2DCsvFile(file_name, 1000, 3640);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            cout << test_data[i][j] << " ";
        }
        cout << endl;
    }

    cout << endl << "END OF CSV READ TEST" << endl << "------------------------------------" << endl << endl;

    return 0;
}

int testCatchment() {
    Catchment c("/home/bernardo/ClionProjects/TriangleModel/TestFiles/updatedOWASAInflowSYN9.csv", 1000, 3640);

    double *streamflow = c.getStreamflow(1);

    for (int i = 0; i < 3640; i++) {
        cout << streamflow[i] << " ";
    }
}




int main() {

//    ::reservoirTest();
//    ::testReadCsv();
//    ::regionContinuityTest();
    ::testCatchment();

    return 0;
}