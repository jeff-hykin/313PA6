/* 
    File: SharedMemoryChannel.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

#include "reqshm.h"
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

// 
//  SharedResourceManager
// 
    // constructors
        SharedResourceManager::SharedResourceManager()
            {
                
            }
        SharedResourceManager::SharedResourceManager(short number_of_avalible_positions, string input_filename, int input_id_across_processes=WTF_3)
            {
                Create(number_of_avalible_positions, input_filename, input_id_across_processes);
            }
        SharedResourceManager::~SharedResourceManager()
            {
                semctl(memory_id, 0, IPC_RMID);
            }
    // methods
        void SharedResourceManager::Create(short number_of_avalible_positions, string input_filename, int input_id_across_processes=WTF_3)
            {
                id_across_processes = input_id_across_processes;
                filename = input_filename;
                // get an inter-process source id for that file (making sure the file is avaliable)
                int inter_process_key_id = GetInterProcessKeyUsingFile(filename, id_across_processes);
                // set the flags
                sembuf sb{0, number_of_avalible_positions, 0};
                memory_id = semget(inter_process_key_id, 1, CREATE_IF_DOESNT_YET_EXIST_FLAG | FAIL_IF_KEY_EXISTS_FLAG | WTF_2);
                if(memory_id < 0)
                    {
                        memory_id = semget(inter_process_key_id, 1, CREATE_IF_DOESNT_YET_EXIST_FLAG | WTF_2);
                    }
                else
                    {
                        semop(memory_id, &sb, 1);
                    }
            }
        void SharedResourceManager::WaitTillResourceIsAvaliableThenTakeIt()
            {
                sembuf sb{0, -1, 0};
                semop(memory_id, &sb, 1);
            }
        void SharedResourceManager::GiveResource()
            {
                sembuf sb{0, 1, 0};
                semop(memory_id, &sb, 1);
            }
    
    

// 
//  SharedMemory255
// 
    // constructors 
        SharedMemory255::SharedMemory255()
            {
            }
        SharedMemory255::~SharedMemory255()
            {
                // FIXME delete file
                // FIXME free up shmid 
            }
    // methods
        void   SharedMemory255::Create(string input_filename)
            {
                filename = input_filename;
                // make the file if it doesnt exist
                ofstream(input_filename.c_str());
                // get an inter-process source id for that file (making sure the file is avaliable)
                int inter_process_key_id = GetInterProcessKeyUsingFile(filename, id_across_processes);
                shared_memory_id = shmget(inter_process_key_id, MAX_MESSAGE_SIZE, WTF_1 | CREATE_IF_DOESNT_YET_EXIST_FLAG);
                // now the memory exists
                memory_exists = true;
            }
        string SharedMemory255::Read()
            {
                // take control of the memory
                memory_manager.WaitTillResourceIsAvaliableThenTakeIt();
                // attach it
                void* pointer_to_data = shmat(shared_memory_id, NULL, 0);
                char c_string[MAX_MESSAGE_SIZE + 1];
                // copy over the data
                memcpy(c_string, pointer_to_data, MAX_MESSAGE_SIZE);
                // add a terminator 
                c_string[MAX_MESSAGE_SIZE] = '\n';
                // detach the memory
                shmdt(pointer_to_data);
                // tell the manager were done using the memory
                memory_manager.GiveResource();
                return c_string;
            }
        void   SharedMemory255::Write(string input_data)
            {
                if (not memory_exists)
                    {
                        puts("Trying to write to SharedMemory255 before the memory has been created.\n filename = " << filename << "\n");
                        exit(1);
                    }
                if (sizeof input_data.c_str() > MAX_MESSAGE_SIZE)
                    {
                        puts("There was a input data being written on the " << filename <<  " SharedMemory255, but the the data was too large\nThe data was " << input_data);
                        exit(1);
                    }
                // take control of the memory
                memory_manager.WaitTillResourceIsAvaliableThenTakeIt();
                // attach it
                void* pointer_to_data = shmat(shared_memory_id, NULL, 0);
                // overwrite the data
                memcpy(pointer_to_data, input_data.c_str(), MAX_MESSAGE_SIZE);
                // detach the memory
                shmdt(pointer_to_data);
                // tell the manager were done using the memory
                memory_manager.GiveResource();
            }




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