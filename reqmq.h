
#ifndef _reqmq_H_
#define _reqmq_H_

#include "reqchannel.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/msg.h>
#include <unistd.h>
using namespace std;



#define MAX_MESSAGE_SIZE 255
struct MsgStruct
    {
        long mailbox_number = 1;
        char data[MAX_MESSAGE_SIZE];
    };

struct Messenger 
    {
        // data
            int id;
            int message_source_id;
            int id_across_processes = 100;
            long mailbox_number = 1;
            string filename;
            MsgStruct package_to_send;
            MsgStruct package_to_receive;
            // TODO, these are no longer needed
            void* data_pointer;
            bool has_data = false;
            long long data_size_in_bytes;
            void* message_type_and_data_pointer;
        // constructors
            Messenger(string, long);
            ~Messenger();
        // methods
            void Send(void* input_data, long input_mailbox_number);
            void* Receive();
    };

class MessageQue : public RequestChannel
    {
    public:
        // Data
            Messenger client_messenger;
            Messenger server_messenger;
            string name      = "";
            string side_name = "";
            RequestChannel::Side side;
        // Constructors
            MessageQue(const string, const RequestChannel::Side);
            ~MessageQue();
        // getters
            string get_name();
        // Methods
            string cread();
            void   cwrite(string _msg);
    };
#undef MAX_MESSAGE_SIZE
#endif