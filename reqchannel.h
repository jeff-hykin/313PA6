
#ifndef _reqchannel_H_
#define _reqchannel_H_

#include <exception>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;


class RequestChannel
    {
    public:
        // Data
            typedef enum
            {
                SERVER_SIDE,
                CLIENT_SIDE
            } Side;

            typedef enum
            {
                READ_MODE,
                WRITE_MODE
            } Mode;
    
        // Data
            string my_name   = "";
            string side_name = "";
            Side   my_side;
            /*  The current implementation uses named pipes. */
            int write_file_descriptor;
            int read_file_descriptor;
        // methods
            virtual string pipe_name(Mode _mode) = 0;
            virtual void   create_pipe(string _pipe_name) = 0;
            virtual void   open_read_pipe(string _pipe_name) = 0;
            virtual void   open_write_pipe(string _pipe_name) = 0;

        // Constructors
            virtual ~RequestChannel() = 0;
        // Methods
            virtual string cread() = 0;
            virtual void   cwrite(string _msg) = 0;
            virtual string name() = 0;
            virtual int    read_fd() = 0;
            virtual int    write_fd() = 0;
    };
#endif
