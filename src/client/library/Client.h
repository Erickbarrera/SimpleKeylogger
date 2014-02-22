#pragma once
#pragma comment(lib, "Ws2_32.lib")
//#define _WIN32_WINNT 0x501
#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

//#ifndef CLIENT_H_INCLUDED
//#define CLIENT_H_INCLUDED

class Client
{
	public:
		Client(void);
		~Client(void);
		SOCKET connectSocket;
		int iResult;  
};
void Listen(SOCKET csock);
void UploadLog(SOCKET csock);
void SendTestMesg(SOCKET csock);


#endif // CLIENT_H_INCLUDED
