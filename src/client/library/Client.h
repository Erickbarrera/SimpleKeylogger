//#define _WIN32_WINNT 0x501

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>

#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

class Client
{
public:
    int iResult;
    SOCKET connectSocket;
    UploadLog();
    Client(void);
    ~Client(void);
};


#endif // CLIENT_H_INCLUDED
