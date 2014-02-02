#include <iostream>
#include <windows.h>

using namespace std;

void CallDLL();
void StartLoopThread();

//This function works but I'm still working on "StartMainLoop"
//StartMainLoop() Creates a new thread which loops to listen for key presses
void StartLoopThread()
{
    HINSTANCE hGetProcIDDLL = LoadLibrary("LogKey.dll");
    FARPROC lpfnGetProcessID = GetProcAddress(HMODULE (hGetProcIDDLL),"StartMainLoop");
    typedef int (__stdcall * pICFUNC)();
    pICFUNC startLoop;
    startLoop = pICFUNC(lpfnGetProcessID);
    startLoop();
    FreeLibrary(hGetProcIDDLL);
}

//Use this as a template for calling functions in a DLL
void CallDLL()
{
    HINSTANCE hGetProcIDDLL = LoadLibrary("LogKey.dll");
    FARPROC lpfnGetProcessID = GetProcAddress(HMODULE (hGetProcIDDLL),"TestDLL");
    typedef int (__stdcall * pICFUNC)();
    pICFUNC TestDLL;
    TestDLL = pICFUNC(lpfnGetProcessID);
    TestDLL();
    FreeLibrary(hGetProcIDDLL);
}

int main()
{
    cout << "Calling DLL..." << endl;
    CallDLL();
    //StartLoopThread();
    cout << "Paused, press ENTER to continue." << endl;
    cin.ignore(1000000);
    return 0;
}


