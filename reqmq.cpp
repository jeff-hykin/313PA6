/* 
    File: MessageQue.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

#include "reqmq.h"
#include <cassert>
#include <cstring>
#include <errno.h>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
using namespace std;

// 
// constants 
// 
#define CREATE_IF_DOESNT_YET_EXIST_FLAG IPC_CREAT
#define FAIL_IF_KEY_EXISTS IPC_EXCL
#define REMOVE_IDENTIFIER_FLAG IPC_RMID
// No idea what these magic numbers are
#define WTF_1 0644
#define WTF_2 0666

// basically a threadsafe cout 
#ifndef puts
#define puts(ARGS) {stringstream converter_to_string; converter_to_string << ARGS; cout << converter_to_string.str(); }
#endif

// 
// Messenger255
//
    // constructors
        Messenger255::Messenger255(string input_filename, long input_mailbox_number) : filename(input_filename), mailbox_number(input_mailbox_number)
            {
                // a warning
                if (mailbox_number <= 0)
                    {
                        cout << "Note mailbox numbers <= 0 have different behavior and the messenger associated with " << input_filename << " has a number of " << mailbox_number << "\n"
                                << "The behavior is:\n"
                                << "If mailbox_number is 0, \n    then the first message in the queue is read.\n" 
                                << "If mailbox_number is greater than 0, \n    then the first message in the queue of type mailbox_number is read, unless MSG_EXCEPT was specified in msgflg, in which case the first message in the queue of type not equal to mailbox_number will be read.\n" 
                                << "If mailbox_number is less than 0, \n    then the first message in the queue with the lowest type less than or equal to the absolute value of mailbox_number will be read\n";
                    }
                package_to_receive.mailbox_number = mailbox_number;
                filename = input_filename;
                // make the file if it doesnt exist
                ofstream(input_filename.c_str());
                // get an inter-process source id for that file (making sure the file is avaliable)
                int inter_process_source_id = ftok(filename.c_str(), id_across_processes);
                if (inter_process_source_id < 0) 
                    {
                        puts("There was an error creating a Messenger255() with the filename of " << filename  << "\n")
                        exit(1);
                    }
                // create the mailing district using the file as the memory source
                mailing_district_id = msgget(inter_process_source_id, CREATE_IF_DOESNT_YET_EXIST_FLAG | WTF_1 );
                if (mailing_district_id < 0)
                    {
                        puts("There was an error using msgget() inside Messenger255() with the filename of " << filename  << "\n")
                        exit(1);
                    }
            }
        Messenger255::~Messenger255() 
            {
                // delete the file (if it exists)
                remove(filename.c_str());
                // delete the id
                msgctl(mailing_district_id, REMOVE_IDENTIFIER_FLAG, NULL);
            }
    // methods
        void Messenger255::Send(string input_data, long input_mailbox_number)
            {
                if (sizeof input_data.c_str() > MAX_MESSAGE_SIZE)
                    {
                        puts("There was a message being sent on the " << filename <<  " messenger, but the message was too large\nThe message was " << input_data);
                        exit(1);
                    }
                // set the mailbox_number
                package_to_send.mailbox_number = input_mailbox_number;
                // copy over the input into the package
                memcpy(package_to_send.data, input_data.c_str(), sizeof package_to_send.data);
                // send the message to the mailbox
                msgsnd(mailing_district_id, &package_to_send, sizeof package_to_send.data, 0);
            }
        string Messenger255::Receive()
            {
                // this changes the data that the data_pointer is pointing to 
                msgrcv(mailing_district_id, &package_to_receive, sizeof package_to_send.data, package_to_receive.mailbox_number, 0);
                // // plus 1 for the null char
                char c_string[MAX_MESSAGE_SIZE + 1];
                // copy the data
                memcpy(c_string, package_to_receive.data, MAX_MESSAGE_SIZE);
                // add the null terminator
                c_string[MAX_MESSAGE_SIZE] = '\0';
                return c_string;
            }




// 
// Constructors
// 
    MessageQue::MessageQue(const string input_name, const RequestChannel::Side input_side) : name(input_name), side(input_side), client_messenger("temp_"+input_name, 'c'), server_messenger("temp_"+input_name, 's')
        {
        }
    
    MessageQue::~MessageQue()
        {
        }

//
// Getters
// 
    string MessageQue::cread           ()
        {
            if (side == SERVER_SIDE) 
                {
                    return server_messenger.Receive();
                }
            else
                {
                    return client_messenger.Receive();
                }
        }

    void   MessageQue::cwrite          (string message)
        {
            if (side == SERVER_SIDE)
                {
                    server_messenger.Send(message, client_messenger.mailbox_number);
                }
            else
                {
                    client_messenger.Send(message, server_messenger.mailbox_number);
                }
        }

#undef CREATE_IF_DOESNT_YET_EXIST_FLAG
#undef FAIL_IF_KEY_EXISTS
#undef REMOVE_IDENTIFIER_FLAG
#undef WTF_1
#undef WTF_2