
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
            string name      = "";
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
            Fifo(const string, const RequestChannel::Side);
            ~Fifo();
        // getters
            string get_name();
            int    get_read_file_descriptor();
            int    get_write_file_descriptor();
        // Methods
            string cread();
            void   cwrite(string _msg);
    };
#endif
