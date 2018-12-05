#include "reqchannel.h"
#include "reqfifo.h"
#include "reqmq.h"
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
#define SharedMemory Fifo
using namespace std;

int             nchannels = 0;
pthread_mutex_t newchannel_lock;
void*           handle_process_loop(void* _channel);
char            ipc_option;

RequestChannel* getChannel(string name) 
    {
        // fifo
        if (ipc_option == 'f') {
            return new Fifo(name, SERVER_SIDE);
        // message queue
        } else if (ipc_option == 'q') {
            return new MessageQue(name, SERVER_SIDE);
        // shared memory
        } else if (ipc_option == 's') {
            return new SharedMemory(name, CLIENT_SIDE);
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

void process_request(RequestChannel* input_channel, string input_request)
    {
        if(input_request.compare(0, 5, "hello") == 0)
            {
                input_channel->cwrite("hello to you too");
            }
        else if(input_request.compare(0, 4, "data") == 0)
            {   usleep(1000 + (rand() % 5000));
                input_channel->cwrite(to_string(rand() % 100));
            }
        else if(input_request.compare(0, 10, "newchannel") == 0)
            {
                process_newchannel(input_channel);
            }
        else
            {
                input_channel->cwrite("unknown request");
            }
    }

void* handle_process_loop(void* input_channel)
    {
        RequestChannel* channel = (RequestChannel*)input_channel;
        // loop until someone sends quit to the control channel
        for(;;)
            {
                string request = channel->cread();
                if(request == "quit")
                    {
                        delete channel;
                        break;
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
        handle_process_loop(control_channel);
    }
