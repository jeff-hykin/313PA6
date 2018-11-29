
#ifndef _reqchannel_H_
#define _reqchannel_H_

#include <exception>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

// basically a threadsafe cout 
#ifndef puts
#include <sstream>
#define puts(ARGS) {stringstream converter_to_string; converter_to_string << ARGS; cout << converter_to_string.str(); }
#endif

#define MAX_MESSAGE_SIZE 255

// helper function
int GetInterProcessKeyUsingFile(string input_filename, int id_across_processes);


class RequestChannel
    {
    public:
        // Data types
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
        // Constructors
            virtual ~RequestChannel() = 0;
        // methods
            virtual string cread                     (      ) = 0;
            virtual void   cwrite                    (string) = 0;
    };
#define CLIENT_SIDE RequestChannel::CLIENT_SIDE
#define SERVER_SIDE RequestChannel::SERVER_SIDE
#define READ_MODE   RequestChannel::READ_MODE
#define WRITE_MODE  RequestChannel::WRITE_MODE
#endif
