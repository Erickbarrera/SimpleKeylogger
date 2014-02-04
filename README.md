#Undetectable Keylogger [POC]
---
---
######Notice:
>This keylogger is considered a proof of concept for our [ethical hacking class final research project.](http://web.eng.fiu.edu/~aperezpo/EEL4789/index.htm)
>It is in **no way** meant to be used for malicious purposes.



#####Project Structure:
* **bin** *(contains binary files for the project)*
* **src** *(contains source files for the project)*
 * **KeyLoggerLibrary** *(contains the code for KeyLoggerLibrary.dll, includes all of the key-logging functionality)*
 * **KeyLoggerLoader** *(contains the code for loader.exe, includes all of the loader functionality)*
						
#####How it Works ****NEEDS FIXING*:
-This is the program that will be compiled as a DLL to be injected into a process
-As of now this program only logs keys and keeps them locally
 Things to do
-Implement winsock to send logs to php server
-Add a file downloader or/and remote shell

#####TODO:
- Make FUD from AV at scantime & runtime
- For loader:
	In Firefox.exe xul.dll hook PostMessageW and PostMessageA
	filter out keypress messages to actual browser control and search control and address var control
		- use window message subclassing to filter out
