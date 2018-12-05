/* 
    File: Fifo.C

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
    Fifo::Fifo(const string input_name, const RequestChannel::Side input_side) : name(input_name), side(input_side)
        {
            // Summary:
                // /* Creates a "local copy" of the channel specified by the given name.
                //  If the channel does not exist, the associated IPC mechanisms are
                //  created. If the channel exists already, this object is associated with the channel.
                //  The channel has two ends, which are conveniently called "SERVER_SIDE" and "CLIENT_SIDE".
                //  If two processes connect through a channel, one has to connect on the server side
                //  and the other on the client side. Otherwise the results are unpredictable.

                //  NOTE: If the creation of the request channel fails (typically happens when too many
                //  request channels are being created) and error message is displayed, and the program
                //  unceremoniously exits.

                //  NOTE: It is easy to open too many request channels in parallel. In most systems,
                //  limits on the number of open files per process limit the number of established
                //  request channels to 125.
                // */
            if(input_side == SERVER_SIDE)
                {
                    open_write_pipe(pipe_name(WRITE_MODE).c_str());
                    open_read_pipe(pipe_name(READ_MODE).c_str());
                }
            else
                {
                    open_read_pipe(pipe_name(READ_MODE).c_str());
                    open_write_pipe(pipe_name(WRITE_MODE).c_str());
                }
        }
    
    Fifo::~Fifo()
        {
            // Summary:
                // /* Destructor of the local copy of the bus. By default, the Server RequestChannel::Side deletes any IPC 
                // mechanisms associated with the channel. */
            close(write_file_descriptor);
            close(read_file_descriptor);
            remove(pipe_name(READ_MODE).c_str());
            remove(pipe_name(WRITE_MODE).c_str());
        }

//
// Getters
// 
    string Fifo::get_name                  () { return name; }
    int    Fifo::get_read_file_descriptor  () { return read_file_descriptor; }
    int    Fifo::get_write_file_descriptor () { return write_file_descriptor; }
// 
// Methods
// 
    string Fifo::pipe_name       (RequestChannel::Mode mode)
        {
            string pipe_name = "fifo_" + name;

            if(side == CLIENT_SIDE)
                {
                    if (mode == READ_MODE) 
                        {
                            pipe_name += "1";
                        }
                    else
                        {
                            pipe_name += "2";
                        }
                }
            else // SERVER_SIDE
                {
                    if(mode == READ_MODE)
                        {
                            pipe_name += "2";
                        }
                    else
                        {
                            pipe_name += "1";
                        }
                }
            return pipe_name;
        }
    void   Fifo::create_pipe     (string pipe_name_argument)
        {
            mkfifo(pipe_name_argument.c_str(), 0600) < 0;
        }

    void   Fifo::open_write_pipe (string pipe_name_argument)
        {
            create_pipe(pipe_name_argument);
            write_file_descriptor = open(pipe_name_argument.c_str(), O_WRONLY);
            if(write_file_descriptor < 0)
                {
                    cout << "Error opening write pipe("<< pipe_name_argument<< ") inside of open_write_pipe" << "\n";
                }
        }

    void   Fifo::open_read_pipe  (string pipe_name_argument)
        {
            create_pipe(pipe_name_argument);
            read_file_descriptor = open(pipe_name_argument.c_str(), O_RDONLY);
            if(read_file_descriptor < 0)
                {
                    cout << "Error opening read pipe("<< pipe_name_argument<< ") inside of open_write_pipe" << "\n";
                    exit(0);
                }
        }

    string Fifo::cread           ()
        {
            // summary:
                // /* Blocking read of data from the channel. Returns a string of characters
                // read from the channel. Returns NULL if read failed. */
            char buf[MAX_MESSAGE];
            if(read(read_file_descriptor, buf, MAX_MESSAGE) <= 0)
                {
                    cerr << "Error in Fifo cread()" << "\n";
                }
            return buf;
        }

    void   Fifo::cwrite          (string msg)
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
    

#undef MAX_MESSAGE