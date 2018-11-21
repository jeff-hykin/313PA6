#include "reqchannel.h"
#include "reqfifo.h"
#include <cassert>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

int             nchannels = 0;
pthread_mutex_t newchannel_lock;
void*           handle_process_loop(void* _channel);
char            ipc_option;

RequestChannel* getChannel(string name) 
    {
        // fifo
        if (ipc_option == 'f') {
            return new Fifo(name, RequestChannel::SERVER_SIDE);
        // message queue
        } else if (ipc_option == 'q') {
            cout << "FIXME, not yet implemented" << "\n";
        // shared memory
        } else if (ipc_option == 's') {
            cout << "FIXME, not yet implemented" << "\n";
        }
        return nullptr;
    }

void process_newchannel(RequestChannel* _channel)
    {
        nchannels++;
        string new_channel_name = "data" + to_string(nchannels) + "_";
        _channel->cwrite(new_channel_name);
        RequestChannel* data_channel = getChannel(new_channel_name);
        pthread_t       thread_id;
        if(pthread_create(&thread_id, NULL, handle_process_loop, data_channel) < 0)
            {
                cout << "Error creating thread in process_newchannel()" << "\n";
            }
    }

void process_request(RequestChannel* _channel, string _request)
    {
        if(_request.compare(0, 5, "hello") == 0)
            {
                _channel->cwrite("hello to you too");
            }
        else if(_request.compare(0, 4, "data") == 0)
            {
                usleep(1000 + (rand() % 5000));
                _channel->cwrite(to_string(rand() % 100));
            }
        else if(_request.compare(0, 10, "newchannel") == 0)
            {
                process_newchannel(_channel);
            }
        else
            {
                _channel->cwrite("unknown request");
            }
    }

void* handle_process_loop(void* _channel)
    {
        RequestChannel* channel = (RequestChannel*)_channel;
        for(;;)
            {
                string request = channel->cread();
                if(request.compare("quit") == 0)
                    {
                        break; // break out of the loop;
                    }
                process_request(channel, request);
            }
    }

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char* argv[])
    {
        newchannel_lock = PTHREAD_MUTEX_INITIALIZER;
        // get the option from the client
        ipc_option = *argv[0];
        RequestChannel* control_channel = getChannel("control");
        handle_process_loop(&control_channel);
        delete control_channel;
    }
