
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



struct Package255
    {
        long mailbox_number = 1;
        char data[MAX_MESSAGE_SIZE];
    };

struct Messenger255
    {
        // data
            int mailing_district_id;
            int id_across_processes = 100; // this is an arbitrary number from what I understand
            long mailbox_number = 1;
            string filename;
            Package255 package_to_send;
            Package255 package_to_receive;
        // constructors
            Messenger255(string, long);
            ~Messenger255();
        // methods
            void Send(string input_data, long input_mailbox_number);
            string Receive();
    };

class MessageQue : public RequestChannel
    {
    public:
        // Data
            Messenger255 client_messenger;
            Messenger255 server_messenger;
            string name      = "";
            RequestChannel::Side side;
        // Constructors
            MessageQue(const string, const RequestChannel::Side);
            ~MessageQue();
        // Methods
            string cread();
            void   cwrite(string _msg);
    };
#endif