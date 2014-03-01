#include <iostream>
#include <Windows.h>
#include "injector.h"
using namespace std;

// TODO
int main()
{
	char cdir[MAX_PATH];
	ZeroMemory(cdir, MAX_PATH);
	GetCurrentDirectory(MAX_PATH, cdir);
	strcat(cdir, "\\library.dll");

	// target process needs to be a 32bit process
	if (InjectDLL("firefox.exe", cdir))
		cout << "Success!" << endl;

	cin.ignore();
	return 0;
}