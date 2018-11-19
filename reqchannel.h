
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
    private:
        // Data
            string my_name   = "";
            string side_name = "";
            Side   my_side;
            /*  The current implementation uses named pipes. */
            int write_file_descriptor;
            int read_file_descriptor;
        // methods
            virtual string pipe_name(Mode _mode);
            virtual void   create_pipe(string _pipe_name);
            virtual void   open_read_pipe(string _pipe_name);
            virtual void   open_write_pipe(string _pipe_name);

    public:
        // Constructors
            RequestChannel();
            RequestChannel(const string _name, const Side _side);
            virtual ~RequestChannel();
        // Methods
            string cread();
            void   cwrite(string _msg);
            string name();
            int    read_fd();
            int    write_fd();
    };
#endif
