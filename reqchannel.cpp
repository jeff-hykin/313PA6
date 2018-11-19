/* 
    File: requestchannel.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "reqchannel.h"
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

string RequestChannel::pipe_name(Mode _mode)
    {
        string pname = "fifo_" + my_name;

        if(my_side == CLIENT_SIDE)
            {
                if(_mode == READ_MODE)
                    pname += "1";
                else
                    pname += "2";
            }
        else
            {
                /* SERVER_SIDE */
                if(_mode == READ_MODE)
                    pname += "2";
                else
                    pname += "1";
            }
        return pname;
    }
void RequestChannel::create_pipe(string _pipe_name)
    {
        mkfifo(_pipe_name.c_str(), 0600) < 0;
    }

void RequestChannel::open_write_pipe(string _pipe_name)
    {
        create_pipe(_pipe_name);
        wfd = open(_pipe_name.c_str(), O_WRONLY);
        if(wfd < 0)
            {
                cerr << "Error creating write pipe RequestChannel::open_write_pipe" << "\n";
            }
    }
void RequestChannel::open_read_pipe(string _pipe_name)
    {
        create_pipe(_pipe_name);
        rfd = open(_pipe_name.c_str(), O_RDONLY);
        if(rfd < 0)
            {
                perror("");
                exit(0);
            }
    }
RequestChannel::RequestChannel(const string _name, const Side _side)
    : my_name(_name), my_side(_side), side_name((_side == RequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT")
    {
        if(_side == SERVER_SIDE)
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

RequestChannel::~RequestChannel()
    {
        close(wfd);
        close(rfd);
        remove(pipe_name(READ_MODE).c_str());
        remove(pipe_name(WRITE_MODE).c_str());
    }




string RequestChannel::cread()
    {
        char buf[MAX_MESSAGE];
        if(read(rfd, buf, MAX_MESSAGE) <= 0)
            {
                cerr << "Error on cread" << "\n";
            }
        string s = buf;
        return s;
    }

void RequestChannel::cwrite(string msg)
    {
        if(msg.size() > MAX_MESSAGE)
            {
                cerr << "Error on cwrite" << "\n";
            }
        // msg.size() + 1 to include the NULL byte
        if(write(wfd, msg.c_str(), msg.size() + 1) < 0)
            { 
                cerr << "Error on cwrite" << "\n";
            }
    }

string RequestChannel::name()
    {
        return my_name;
    }

int RequestChannel::read_fd()
    {
        return rfd;
    }

int RequestChannel::write_fd()
    {
        return wfd;
    }

#undef MAX_MESSAGE