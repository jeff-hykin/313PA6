#include "BoundedBuffer.h"
#include <queue>
#include <string>
#include <iostream>
using namespace std;

BoundedBuffer::BoundedBuffer()
    {
    }

BoundedBuffer::BoundedBuffer(int input_max_size)
    {
        max_size = input_max_size || 1;
    }

BoundedBuffer::~BoundedBuffer()
    {
    }

int BoundedBuffer::size()
    {
        pthread_mutex_lock(&lock);
        int size = queue_of_strings.size();
        pthread_mutex_unlock(&lock);
        return size;
    }

void BoundedBuffer::push(string string_input)
    {
        // if max size, then wait for size change
        CHECK:
        pthread_mutex_lock(&lock);
        if (queue_of_strings.size() < max_size)
            {
                queue_of_strings.push(string_input);
                pthread_mutex_unlock(&lock);
                pthread_cond_signal(&not_empty);
            }
        else
            {
                pthread_mutex_unlock(&lock);
                pthread_cond_wait(&not_full, &lock);
                goto CHECK;
            }
    }

string BoundedBuffer::pop()
    {
        string output;
        CHECK:
        pthread_mutex_lock(&lock);
        if (queue_of_strings.size() > 0)
            {
                output = queue_of_strings.front();
                queue_of_strings.pop();
                pthread_mutex_unlock(&lock);
                pthread_cond_signal(&not_full);
            }
        else
            {
                pthread_mutex_unlock(&lock);
                pthread_cond_wait(&not_empty, &lock);
                goto CHECK;
            }
        return output;
    }