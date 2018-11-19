/* 
    File: FifoRequestChannel.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

#include "reqfifo.h"
#include <cassert>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

#define MAX_MESSAGE 255

// 
// Constructors
// 
    FifoRequestChannel::FifoRequestChannel(const std::string _name, const RequestChannel::Side _side) : my_name(_name), my_side(_side), side_name((_side == RequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT")
        {
            // Summary:
                // /* Creates a "local copy" of the channel specified by the given name.
                //  If the channel does not exist, the associated IPC mechanisms are
                //  created. If the channel exists already, this object is associated with the channel.
                //  The channel has two ends, which are conveniently called "SERVER_SIDE" and "RequestChannel::CLIENT_SIDE".
                //  If two processes connect through a channel, one has to connect on the server side
                //  and the other on the client side. Otherwise the results are unpredictable.

                //  NOTE: If the creation of the request channel fails (typically happens when too many
                //  request channels are being created) and error message is displayed, and the program
                //  unceremoniously exits.

                //  NOTE: It is easy to open too many request channels in parallel. In most systems,
                //  limits on the number of open files per process limit the number of established
                //  request channels to 125.
                // */
            if(_side == RequestChannel::SERVER_SIDE)
                {
                    open_write_pipe(pipe_name(RequestChannel::WRITE_MODE).c_str());
                    open_read_pipe(pipe_name(RequestChannel::READ_MODE).c_str());
                }
            else
                {
                    open_read_pipe(pipe_name(RequestChannel::READ_MODE).c_str());
                    open_write_pipe(pipe_name(RequestChannel::WRITE_MODE).c_str());
                }
        }
    
    FifoRequestChannel::~FifoRequestChannel()
        {
            // Summary:
                // /* Destructor of the local copy of the bus. By default, the Server RequestChannel::Side deletes any IPC 
                // mechanisms associated with the channel. */
            close(write_file_descriptor);
            close(read_file_descriptor);
            remove(pipe_name(RequestChannel::READ_MODE).c_str());
            remove(pipe_name(RequestChannel::WRITE_MODE).c_str());
        }

// 
// Methods
// 
    std::string FifoRequestChannel::pipe_name(RequestChannel::Mode _mode)
        {
            std::string pname = "fifo_" + my_name;

            if(my_side == RequestChannel::CLIENT_SIDE)
                {
                    if(_mode == RequestChannel::READ_MODE)
                        pname += "1";
                    else
                        pname += "2";
                }
            else
                {
                    /* SERVER_SIDE */
                    if(_mode == RequestChannel::READ_MODE)
                        pname += "2";
                    else
                        pname += "1";
                }
            return pname;
        }
    void FifoRequestChannel::create_pipe(string _pipe_name)
        {
            mkfifo(_pipe_name.c_str(), 0600) < 0;
        }

    void FifoRequestChannel::open_write_pipe(string _pipe_name)
        {
            create_pipe(_pipe_name);
            write_file_descriptor = open(_pipe_name.c_str(), O_WRONLY);
            if(write_file_descriptor < 0)
                {
                    cout << "Error opening new pipe("<< _pipe_name<< ") inside of open_write_pipe" << "\n";
                }
        }

    void FifoRequestChannel::open_read_pipe(string _pipe_name)
        {
            create_pipe(_pipe_name);
            read_file_descriptor = open(_pipe_name.c_str(), O_RDONLY);
            if(read_file_descriptor < 0)
                {
                    perror("");
                    exit(0);
                }
        }

    string FifoRequestChannel::cread()
        {
            // summary:
                // /* Blocking read of data from the channel. Returns a string of characters
                // read from the channel. Returns NULL if read failed. */
            char buf[MAX_MESSAGE];
            if(read(read_file_descriptor, buf, MAX_MESSAGE) <= 0)
                {
                    cerr << "Error in cread()" << "\n";
                }
            string s = buf;
            return s;
        }

    void FifoRequestChannel::cwrite(string msg)
        {
            if(msg.size() > MAX_MESSAGE)
                {
                    cerr << "Error in cwrite()" << "\n";
                }
            // msg.size() + 1 to include the NULL byte
            if(write(write_file_descriptor, msg.c_str(), msg.size() + 1) < 0)
                { 
                    cerr << "Error in cwrite()" << "\n";
                }
        }

    std::string FifoRequestChannel::name()
        {
            /* Returns the name of the request channel. */
            return my_name;
        }

    int FifoRequestChannel::read_fd()
        {
            /* Returns the file descriptor used to read from the channel. */
            return read_file_descriptor;
        }

    int FifoRequestChannel::write_fd()
        {
            /* Returns the file descriptor used to write to the channel. */
            return write_file_descriptor;
        }

#undef MAX_MESSAGE