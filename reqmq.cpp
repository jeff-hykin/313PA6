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
#define MAX_MESSAGE_SIZE 255
auto CREATE_IF_DOESNT_YET_EXIST_FLAG = IPC_CREAT;
auto FAIL_IF_KEY_EXISTS = IPC_EXCL;
auto REMOVE_IDENTIFIER_FLAG = IPC_RMID;
// No idea what these magic numbers are
#define WTF_1 0644
#define WTF_2 0666


#define puts(ARGS) {stringstream converter_to_string; converter_to_string << ARGS; cout << converter_to_string.str(); }


// 
// Messenger
//
    // constructors
        Messenger::Messenger(string input_filename, long input_mailbox_number) : data_size_in_bytes(MAX_MESSAGE_SIZE), filename(input_filename)
            {
                cout << "input_filename = " << (input_filename) << "\n";
                mailbox_number = input_mailbox_number;
                // just add a warning
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
                // create the file, TODO make this not rely on system()/linux 
                string system_command = "touch " + input_filename;
                system(system_command.c_str());
                // get a source id
                message_source_id = ftok(filename.c_str(), id_across_processes);
                if (message_source_id < 0) 
                    {
                        cerr << "There was an error creating a Messenger() with the filename of " << filename  << "\n";
                        exit(1);
                    }
                id = msgget(message_source_id, CREATE_IF_DOESNT_YET_EXIST_FLAG | WTF_1 );
                if (id < 0)
                    {
                        cerr << "There was an error using msgget() inside Messenger() with the filename of " << filename  << "\n";
                        exit(1);
                    }
            }
        Messenger::~Messenger() 
            {
                msgctl(id, REMOVE_IDENTIFIER_FLAG, NULL);
                if (has_data)
                    {
                        delete message_type_and_data_pointer;
                    }
            }
    // methods
        void Messenger::Send(void* input_data, long input_mailbox_number)
            {
                // set the mailbox_number
                package_to_send.mailbox_number = input_mailbox_number;
                // copy over the input into the package
                memcpy(package_to_send.data, input_data, data_size_in_bytes);
                // FIXME, Debugging
                if ((string)package_to_send.data == "unknown request")
                    {
                        cerr << "unknown request " << "\n";
                        exit(1);
                    }
                puts( "    " << filename << (mailbox_number == 's'? " server ":" client ") << "SENDING \"" << package_to_send.data << "\" to " << (package_to_send.mailbox_number == 's'? " server ":" client ") << "\n");
                msgsnd(id, &package_to_send, data_size_in_bytes, 0);
                MsgStruct package_to_receive;
                package_to_receive.mailbox_number = package_to_send.mailbox_number;
                auto error = msgrcv(id, &package_to_receive, data_size_in_bytes, 0, 0);
                // return the data_pointer after the changes are made
                puts( "    " << filename << (package_to_receive.mailbox_number == 's'? " server ":" client ") << "RECEIVED \"" << package_to_receive.data << "\"\n");
            }
        void* Messenger::Receive()
            {
                // this changes the data that the data_pointer is pointing to 
                puts( "    " << filename << (package_to_receive.mailbox_number == 's'? " server ":" client ") << " TRYING TO RECEIVE \n");
                msgrcv(id, &package_to_receive, data_size_in_bytes, package_to_receive.mailbox_number, 0);
                // return the data_pointer after the changes are made
                puts( "    " << filename << (mailbox_number == 's'? " server ":" client ") << "RECEIVED \"" << package_to_receive.data << "\"\n");
                return package_to_receive.data;
            }
        void Messenger::SetDataAndMessageType(void* input_data)
            {
                // delete the old message data before creating a new thing
                if (has_data)
                    {
                        delete message_type_and_data_pointer;
                    }
                int room_for_message_type = sizeof(long);
                message_type_and_data_pointer = new char(data_size_in_bytes + room_for_message_type);
                // data is just a few bytes down from message_type_and_data_pointer
                data_pointer = (void*)(((long)message_type_and_data_pointer) + room_for_message_type);
                memcpy(data_pointer, input_data, data_size_in_bytes);
                has_data = true;
            }




// 
// Constructors
// 
    MessageQue::MessageQue(const string input_name, const RequestChannel::Side input_side) : name(input_name), side(input_side), client_messenger("temp_"+input_name, 'c'), server_messenger("temp_"+input_name, 's')
        {
            side_name = (input_side == SERVER_SIDE) ? "SERVER" : "CLIENT";
        }
    
    MessageQue::~MessageQue()
        {
            
        }

//
// Getters
// 
    string MessageQue::get_name        () { return name; }
    string MessageQue::cread           ()
        {
            cout << "cread from " << side_name  << "\n";
            void* data;
            if (side_name == "SERVER") 
                {
                    data = server_messenger.Receive();
                }
            else
                {
                    data = client_messenger.Receive();
                }
            // // plus 1 for the null char
            char c_string[MAX_MESSAGE_SIZE + 1];
            // copy the data
            memcpy(c_string, data, MAX_MESSAGE_SIZE);
            // add the null terminator
            c_string[MAX_MESSAGE_SIZE] = '\0';
            return c_string;
        }

    void   MessageQue::cwrite          (string msg)
        {
            cout << "cwrite from " << side_name << "\n";
            void* data = (void*)msg.c_str();
            if (side_name == "SERVER") 
                {
                    server_messenger.Send(data, client_messenger.mailbox_number);
                }
            else
                {
                    client_messenger.Send(data, server_messenger.mailbox_number);
                }
        }
    

#undef MAX_MESSAGE_SIZE