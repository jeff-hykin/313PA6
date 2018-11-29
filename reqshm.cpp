/* 
    File: SharedMemory.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

#include "reqshm.h"
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
    SharedMemory::SharedMemory(const string input_name, const RequestChannel::Side input_side) : name(input_name), side(input_side)
        {
        }
    
    SharedMemory::~SharedMemory()
        {
        }

// 
// Methods
// 
    string SharedMemory::cread           ()
        {
            
        }

    void   SharedMemory::cwrite          (string msg)
        {
            
        }
    

#undef MAX_MESSAGE