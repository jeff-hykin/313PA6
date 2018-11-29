#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <stdio.h>
#include <string>
#include <pthread.h>
#include <reqchannel.h>
using namespace std;

// 
// Contstants
// 
#define CREATE_IF_DOESNT_YET_EXIST_FLAG IPC_CREAT
#define FAIL_IF_KEY_EXISTS IPC_EXCL
#define REMOVE_IDENTIFIER_FLAG IPC_RMID
// No idea what these magic numbers are
#define WTF_1 0644
#define WTF_2 0666
#define WTF_3 101

class SharedResourceManager
    {
    private:
        int memory_id;
        string filename;
        int id_across_processes = WTF_3; // this is an arbitrary number from what I understand

    public:
        // constructors
            SharedResourceManager()
                {
                    
                }
            SharedResourceManager(short number_of_avalible_positions, string input_filename, int input_id_across_processes=WTF_3)
                {
                    Create(number_of_avalible_positions, input_filename, input_id_across_processes);
                }
            ~SharedResourceManager()
                {
                    semctl(memory_id, 0, IPC_RMID);
                }
        // methods
            void Create(short number_of_avalible_positions, string input_filename, int input_id_across_processes=WTF_3)
                {
                    id_across_processes = input_id_across_processes;
                    filename = input_filename;
                    // get an inter-process source id for that file (making sure the file is avaliable)
                    int inter_process_key_id = GetInterProcessKeyUsingFile(filename, id_across_processes);
                    // set the flags
                    sembuf sb{0, number_of_avalible_positions, 0};
                    memory_id = semget(inter_process_key_id, 1, CREATE_IF_DOESNT_YET_EXIST_FLAG | FAIL_IF_KEY_EXISTS | WTF_2);
                    if(memory_id < 0)
                        {
                            memory_id = semget(inter_process_key_id, 1, CREATE_IF_DOESNT_YET_EXIST_FLAG | WTF_2);
                        }
                    else
                        {
                            semop(memory_id, &sb, 1);
                        }
                }
            void WaitTillResourceIsAvaliableThenTakeIt()
                {
                    sembuf sb{0, -1, 0};
                    semop(memory_id, &sb, 1);
                }
            void GiveResource()
                {
                    sembuf sb{0, 1, 0};
                    semop(memory_id, &sb, 1);
                }
    };

#undef CREATE_IF_DOESNT_YET_EXIST_FLAG
#undef FAIL_IF_KEY_EXISTS
#undef REMOVE_IDENTIFIER_FLAG
#undef WTF_1
#undef WTF_2