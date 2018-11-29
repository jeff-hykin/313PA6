
#ifndef _reqshm_H_
#define _reqshm_H_

#include "reqchannel.h"
#include <sys/sem.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

struct SharedResourceManager
    {
        // data
            int memory_id;
            string filename;
            int id_across_processes = WTF_3; // this is an arbitrary number from what I understand

        // constructors
            SharedResourceManager();
            SharedResourceManager(short number_of_avalible_positions, string input_filename, int input_id_across_processes);
            ~SharedResourceManager();
        // methods
            void Create(short number_of_avalible_positions, string input_filename, int input_id_across_processes);
            void WaitTillResourceIsAvaliableThenTakeIt();
            void GiveResource();
    };

struct SharedMemory255
    {
        // data
            void* location;
            int id_across_processes = 103; // this is an arbitrary number from what I understand
            int shared_memory_id;
            bool memory_exists = false;
            SharedResourceManager memory_manager;
            string filename;
        // constructors 
            SharedMemory255();
            ~SharedMemory255();
        // methods
            void Create(string input_filename);
            string Read();
            void Write(string input_data);
    };

struct SharedMemoryQue
    {
        // data
            SharedResourceManager not_empty_manager;
            SharedResourceManager not_full_manager;
            SharedMemory255 memory;
        // methods
            void push(string);
            string pop();
    };


struct SharedMemoryChannel : public RequestChannel
    {
        // Data
            string name      = "";
            RequestChannel::Side side;

        // Constructors
            SharedMemoryChannel(const string, const RequestChannel::Side);
            ~SharedMemoryChannel();
        // Methods
            string cread();
            void   cwrite(string);
    };
#endif
