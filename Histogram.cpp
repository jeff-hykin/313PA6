#include <iomanip>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <sstream>
using namespace std;

#include "Histogram.h"

Histogram::Histogram()
{
    // initilize the hist array
    for(int i = 0; i < 3; i++)
        {
            memset(hist[i], 0, 10 * sizeof(int));
            locks.push_back(PTHREAD_MUTEX_INITIALIZER);
        }
    map["data John Smith"] = 0;
    map["data Jane Smith"] = 1;
    map["data Joe Smith"]  = 2;

    names.push_back("John Smith");
    names.push_back("Jane Smith");
    names.push_back("Joe Smith");
}

void Histogram::update(string request, string response)
{
    int person_index = map[request];
    pthread_mutex_lock(&locks[person_index]);
    hist[person_index][stoi(response) / 10]++;
    pthread_mutex_unlock(&locks[person_index]);
}

void Histogram::print()
{
    stringstream output;
    output << setw(10) << right << "Range";
    for(int j = 0; j < 3; j++)
        {
            output << setw(15) << right << names[j];
        }
    output << endl;
    output << "----------------------------------------------------------" << endl;
    int sum[3];
    memset(sum, 0, 3 * sizeof(int));
    for(int i = 0; i < 10; i++)
        {
            string range = "[" + to_string(i * 10) + " - " + to_string((i + 1) * 10 - 1) + "]:";
            output << setw(10) << right << range;
            for(int j = 0; j < 3; j++)
                {
                    output << setw(15) << right << hist[j][i];
                    sum[j] += hist[j][i];
                }
            output << endl;
        }
    output << "----------------------------------------------------------" << endl;
    output << setw(10) << right << "Totals:";
    for(int j = 0; j < 3; j++)
        {
            output << setw(15) << right << sum[j];
        }
    output << endl;
    // output everything at once
    cout << output.str();
}