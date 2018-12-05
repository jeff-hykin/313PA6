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
        SharedResourceManager::SharedResourceManager(string input_filename, short number_of_avalible_positions, int input_id_across_processes=WTF_3)
            {
                Create(input_filename, number_of_avalible_positions, input_id_across_processes);
            }
        SharedResourceManager::~SharedResourceManager()
            {
                // free up the memory_id
                semctl(memory_id, 0, REMOVE_IDENTIFIER_FLAG);
                // delete the file
                remove(filename.c_str());
            }
    // methods
        void SharedResourceManager::Create(string input_filename, short number_of_avalible_positions=0, int input_id_across_processes=WTF_3)
            {
                puts(input_filename + " Creating SharedResourceManager");
                id_across_processes = input_id_across_processes;
                filename = input_filename;
                // get an inter-process source id for that file (making sure the file is avaliable)
                int inter_process_key_id = GetInterProcessKeyUsingFile(filename, id_across_processes);
                // first check to see if it already exists
                memory_id = semget(inter_process_key_id, 1, CREATE_IF_DOESNT_YET_EXIST_FLAG | FAIL_IF_KEY_EXISTS_FLAG | WTF_2);
                // if it already exists then just get the id 
                if(memory_id < 0)
                    {
                        memory_id = semget(inter_process_key_id, 1, CREATE_IF_DOESNT_YET_EXIST_FLAG | WTF_2);
                    }
                // if its completely new
                else
                    {
                        // initilize it with the number_of_avalible_positions
                        sembuf sb{0, number_of_avalible_positions, 0};
                        semop(memory_id, &sb, 1);
                    }
            }
        void SharedResourceManager::WaitTillResourceIsAvaliableThenTakeIt()
            {
                puts(filename + " WaitTillResourceIsAvaliableThenTakeIt");
                sembuf sb{0, -1, 0};
                semop(memory_id, &sb, 1);
                puts(filename + " Taking");
            }
        void SharedResourceManager::GiveResource()
            {
                puts(filename + " GiveResource");
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
                // free up the memory
                shmctl(shared_memory_id, REMOVE_IDENTIFIER_FLAG, NULL);
                // delete the file
                remove(filename.c_str());
            }
    // methods
        void   SharedMemory255::Create(string input_filename)
            {
                puts(input_filename + " Creating SharedMemory255");
                filename = input_filename;
                // get an inter-process source id for that file (making sure the file is avaliable)
                int inter_process_key_id = GetInterProcessKeyUsingFile(filename, id_across_processes);
                shared_memory_id = shmget(inter_process_key_id, MAX_MESSAGE_SIZE, WTF_1 | CREATE_IF_DOESNT_YET_EXIST_FLAG);
                // create the read_and_write manager and initilize it with one space
                read_and_write.Create(filename+"_read_and_write", 1); 
                // now the memory exists
                memory_exists = true;
            }
        string SharedMemory255::Read()
            {
                puts(filename + " Read");
                // take control of the memory
                read_and_write.WaitTillResourceIsAvaliableThenTakeIt();
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
                read_and_write.GiveResource();
                return c_string;
            }
        void   SharedMemory255::Write(string input_data)
            {
                puts(filename + " Write");
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
                read_and_write.WaitTillResourceIsAvaliableThenTakeIt();
                // attach it
                void* pointer_to_data = shmat(shared_memory_id, NULL, 0);
                // overwrite the data
                memcpy(pointer_to_data, input_data.c_str(), MAX_MESSAGE_SIZE);
                puts(filename + " after memcpy");
                // detach the memory
                shmdt(pointer_to_data);
                // tell the manager were done using the memory
                read_and_write.GiveResource();
            }




//
// Shared Memory Que
// 
    void SharedMemoryQue::Create(string input_filename)
        {
            puts(input_filename + " Creating SharedMemoryQue");
            memory.Create( input_filename+"_memory" );
            not_empty_manager.Create( input_filename+"_not_empty_manager" );
            // by default its not full, so initilize it with one spot
            not_full_manager.Create( input_filename+"_not_full_manager" , 1);
        }
    void SharedMemoryQue::Push(string message)
        {
            puts(memory.filename+" Push");
            not_full_manager.WaitTillResourceIsAvaliableThenTakeIt();
            memory.Write(message);
            not_empty_manager.GiveResource();
        }
    string SharedMemoryQue::Pop()
        {
            puts(memory.filename+" Pop");
            not_empty_manager.WaitTillResourceIsAvaliableThenTakeIt();
            string output = memory.Read();
            not_full_manager.GiveResource();
            return output;
        }
// 
// SharedMemoryChannel
// 
    // 
    // Constructors
    // 
        SharedMemoryChannel::SharedMemoryChannel(const string input_name, const RequestChannel::Side input_side) : name("temp_"+input_name), side(input_side)
            {
                puts(input_name + " Creating SharedMemoryChannel");
                server_que.Create(name+"_server_que");
                client_que.Create(name+"_client_que");
            }
        
        SharedMemoryChannel::~SharedMemoryChannel()
            {
            }

    // 
    // Methods
    // 
        string SharedMemoryChannel::cread           ()
            {
                puts(name + " cread");
                if (side == SERVER_SIDE)
                    {
                        return server_que.Pop();
                    }
                else
                    {
                        return client_que.Pop();
                    }
            }

        void   SharedMemoryChannel::cwrite          (string message)
            {
                puts(name + " cwrite");
                if (side == SERVER_SIDE)
                    {
                        client_que.Push(message);
                    }
                else
                    {
                        server_que.Push(message);
                    }
            }