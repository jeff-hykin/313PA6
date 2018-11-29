/* 
    File: SharedMemoryChannel.C

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
            SharedResourceManager buffer_manager;
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
        buffer_manager.GiveResource();
        // FIXME push stuff here
    }

string KernelBoundedBuffer::pop()
    {
        buffer_manager.WaitTillResourceIsAvaliableThenTakeIt();
        string output = "FIXME, 02940";
        // FIXME pop stuff here
        return output;
    }
    
template <class ANYTYPE>
class SharedMemory
    {
        // data
            void* location;
            int id_across_processes = 103; // this is an arbitrary number from what I understand
            SharedResourceManager memory_manager;
            string filename;
            ANYTYPE output;
        // constructors 
            SharedMemory()
                {
                }
            //  TODO destructor
        // methods
            void SetLocation(string input_filename)
                {
                    filename = input_filename;
                    // make the file if it doesnt exist
                    ofstream(input_filename.c_str());
                    // get an inter-process source id for that file (making sure the file is avaliable)
                    int inter_process_key_id = GetInterProcessKeyUsingFile(filename, id_across_processes);
                    
                }
            ANYTYPE Read()
                {
                    // attach the memory
                    // get the memory_manager for it
                    memory_manager.WaitTillResourceIsAvaliableThenTakeIt();
                    // wait
                    // copy data
                    ANYTYPE* data_pointer = location;
                    ANYTYPE output = *data_pointer;
                    // done using memory
                    memory_manager.GiveResource();
                    return output;
                }
            void Write(ANYTYPE input)
                {
                    memory_manager.WaitTillResourceIsAvaliableThenTakeIt();
                    // FIXME do the writing here
                    memory_manager.GiveResource();
                }
    };

// 
// Constructors
// 
    SharedMemoryChannel::SharedMemoryChannel(const string input_name, const RequestChannel::Side input_side) : name(input_name), side(input_side)
        {
        }
    
    SharedMemoryChannel::~SharedMemoryChannel()
        {
        }

// 
// Methods
// 
    string SharedMemoryChannel::cread           ()
        {
            
        }

    void   SharedMemoryChannel::cwrite          (string msg)
        {
            
        }