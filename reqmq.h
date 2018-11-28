
#ifndef _reqmq_H_
#define _reqmq_H_

#include "reqchannel.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;


class MessageQue : public RequestChannel
    {
    private:
        // Data
            string name      = "";
            string side_name = "";
            RequestChannel::Side side;
            /*  The current implementation uses named pipes. */
            int write_file_descriptor;
            int read_file_descriptor;
        // methods
            string pipe_name(RequestChannel::Mode _mode);
            void   create_pipe     (string);
            void   open_read_pipe  (string);
            void   open_write_pipe (string);

    public:
        // Constructors
            MessageQue(const string, const RequestChannel::Side);
            ~MessageQue();
        // getters
            string get_name();
            int    get_read_file_descriptor();
            int    get_write_file_descriptor();
        // Methods
            string cread();
            void   cwrite(string _msg);
    };
#endif
