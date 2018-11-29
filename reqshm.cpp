/* 
    File: SharedMemory.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

#include "reqshm.h"
#include "KernelSemaphore.h"
#include <cassert>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>

using namespace std;

class KernelBoundedBuffer
    {
    public:
        // data
            KernelSemaphore semaphore;
            queue<string> queue_of_strings;
            int max_size = 0;

        // constructors
            KernelBoundedBuffer();
            KernelBoundedBuffer(int);
            ~KernelBoundedBuffer();
        // methods
            void push(string);
            string pop();
    };

KernelBoundedBuffer::KernelBoundedBuffer()
    {
    }

KernelBoundedBuffer::KernelBoundedBuffer(int input_max_size)
    {
        max_size = input_max_size || 1;
    }

KernelBoundedBuffer::~KernelBoundedBuffer()
    {
    }

void KernelBoundedBuffer::push(string string_input)
    {
        semaphore.Increment();
        // FIXME push stuff here
    }

string KernelBoundedBuffer::pop()
    {
        semaphore.Decrement();
        string output = "FIXME, 02940";
        // FIXME pop stuff here
        return output;
    }

// 
// Constructors
// 
    SharedMemory::SharedMemory(const string input_name, const RequestChannel::Side input_side) : name(input_name), side(input_side)
        {
        }
    
    SharedMemory::~SharedMemory()
        {
        }

// 
// Methods
// 
    string SharedMemory::cread           ()
        {
            
        }

    void   SharedMemory::cwrite          (string msg)
        {
            
        }