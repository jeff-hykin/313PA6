
#ifndef _reqchannel_H_
#define _reqchannel_H_

#include <exception>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;


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
            virtual string pipe_name                 (Mode  ) = 0;
            virtual void   create_pipe               (string) = 0;
            virtual void   open_read_pipe            (string) = 0;
            virtual void   open_write_pipe           (string) = 0;
            virtual string cread                     (      ) = 0;
            virtual void   cwrite                    (string) = 0;
    };
#define CLIENT_SIDE RequestChannel::CLIENT_SIDE
#define SERVER_SIDE RequestChannel::SERVER_SIDE
#define READ_MODE   RequestChannel::READ_MODE
#define WRITE_MODE  RequestChannel::WRITE_MODE
#endif
