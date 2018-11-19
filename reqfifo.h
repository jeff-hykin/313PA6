
#ifndef _reqfifo_H_
#define _reqfifo_H_

#include "reqchannel.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;


class Fifo : public RequestChannel
    {
    private:
        // Data
            string my_name   = "";
            string side_name = "";
            RequestChannel::Side my_side;
            /*  The current implementation uses named pipes. */
            int write_file_descriptor;
            int read_file_descriptor;
        // methods
            string pipe_name(RequestChannel::Mode _mode);
            void   create_pipe(string _pipe_name);
            void   open_read_pipe(string _pipe_name);
            void   open_write_pipe(string _pipe_name);

    public:
        // Constructors
            Fifo(const string _name, const RequestChannel::Side _side);
            ~Fifo();
        // Methods
            string cread();
            void   cwrite(string _msg);
            string name();
            int    read_fd();
            int    write_fd();
    };
#endif
