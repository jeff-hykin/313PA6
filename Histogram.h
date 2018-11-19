#ifndef Histogram_h
#define Histogram_h

#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include <pthread.h>
using namespace std;

class Histogram
    {
    private:
        // data
            vector<pthread_mutex_t>    locks;
            int                        hist[3][10]; // histogram for each person with 10 bins each
            unordered_map<string, int> map;         // person name to index mapping
            vector<string>             names;       // names of the 3 persons
    public:
        // constructor
            Histogram();
        // methods
            void update(string, string); // updates the histogram
            void print();                // prints the histogram
    };

#endif
