#include <iostream>
#include <Windows.h>
#include "injector.h"
using namespace std;

int main()
{
	// set target
	char* target_process = "firefox.exe";
	
	// get current directory
	char cdir[MAX_PATH];
	ZeroMemory(cdir, MAX_PATH);
	GetCurrentDirectory(MAX_PATH, cdir);
	strcat(cdir, "\\library.dll");

	// look for target process
	cout << "Waiting for " << target_process << "..." << endl;
	HANDLE hproc;
	do
	{
		hproc = FindProcess(target_process);
	} while (hproc == NULL);

	// found target, inject library
	cout << target_process << " found! Injecting library..." << endl;
	if (InjectDLL(hproc, cdir))
	{
		CloseHandle(hproc);
		cout << "Success!" << endl;
	}
	else
		cout << "Failed!" << endl;

	cin.ignore();
	return 0;
}