#ifndef BoundedBuffer_h
#define BoundedBuffer_h

#include <queue>
#include <stdio.h>
#include <string>
#include <pthread.h>
using namespace std;


class BoundedBuffer
    {
    public:
        // data
            queue<string> queue_of_strings;
            pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
            pthread_cond_t  not_empty;
            pthread_cond_t  not_full;
            int max_size = 0;

        // constructors
            BoundedBuffer();
            BoundedBuffer(int);
            ~BoundedBuffer();
        // methods
            int size();
            void push(string);
            string pop();
    };

#endif /* BoundedBuffer_h */
