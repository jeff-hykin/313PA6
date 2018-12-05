
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
#define puts(ARGS) {stringstream converter_to_string; converter_to_string << ARGS; cout << converter_to_string.str() << "\n"; }
#endif

// 
// Contstants
// 
#define MAX_MESSAGE_SIZE 255
#define CREATE_IF_DOESNT_YET_EXIST_FLAG IPC_CREAT
#define FAIL_IF_KEY_EXISTS_FLAG IPC_EXCL
#define REMOVE_IDENTIFIER_FLAG IPC_RMID
// No idea what these magic numbers are
#define WTF_1 0644
#define WTF_2 0666
#define WTF_3 101

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
