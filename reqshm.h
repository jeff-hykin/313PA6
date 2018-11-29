
#ifndef _reqshm_H_
#define _reqshm_H_

#include "reqchannel.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;


class SharedMemoryChannel : public RequestChannel
    {
    public:
        // Data
            string name      = "";
            RequestChannel::Side side;

        // Constructors
            SharedMemoryChannel(const string, const RequestChannel::Side);
            ~SharedMemoryChannel();
        // Methods
            string cread();
            void   cwrite(string);
    };
#endif
