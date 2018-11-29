
#ifndef _reqshm_H_
#define _reqshm_H_

#include "reqchannel.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;


class SharedMemory : public RequestChannel
    {
    public:
        // Data
            string name      = "";
            RequestChannel::Side side;

        // Constructors
            SharedMemory(const string, const RequestChannel::Side);
            ~SharedMemory();
        // Methods
            string cread();
            void   cwrite(string);
    };
#endif
