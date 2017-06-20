#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;


int main(int argc, char *args[]) {
    // char *file_name = "test_read_2d.csv";
    // vector <vector <double> > data;
    // ifstream infile(file_name);

    // while (infile)
    // {
    //   string s;
    //   if (!getline( infile, s )) break;

    //   istringstream ss( s );
    //   vector <double> record;

    //   while (ss)
    //   {
    //     string s;
    //     if (!getline( ss, s, ',' )) break;
    //     record.push_back( stof(s) );
    //   }

    //   data.push_back( record );
    // }
    // if (!infile.eof())
    // {
    //   cerr << "Could not read file " << file_name << "\n";
    // }

    // for (vector<double> v : data)
    // {
    // 	for (double s : v)
    // 		cout << s << " ";
    // 	cout << endl;
    // }

    vector<double> v(5, 5);
    double *d = new double[5];

    std:
    copy(v.begin(), v.end(), d);

    for (int i = 0; i < 5; i++)
        cout << d[i] << endl;

    return 0;
}