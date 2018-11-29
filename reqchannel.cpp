/* 
    File: requestchannel.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11
*/

#include "reqchannel.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>


int GetInterProcessKeyUsingFile(string input_filename, int id_across_processes=100)
    {
        // make the file if it doesnt exist
        ofstream(input_filename.c_str());
        // get an inter-process source id for that file (making sure the file is avaliable)
        int inter_process_key_id = ftok(input_filename.c_str(), id_across_processes);
        if (inter_process_key_id < 0)
            {
                puts("There was an error in GetInterProcessKeyUsingFile() with the filename of " << input_filename  << " and an id of " << id_across_processes << "\n");
                exit(1);
            }
        return inter_process_key_id;
    }
// 
// Constructors
// 
    // code breaks if you remove this 
    // it shouldnt because its a pure vitural function
    // it shouldnt because it literally does nothing
    // but it breaks anyways ¯\_(ツ)_/¯
    RequestChannel::~RequestChannel()
        {
        }