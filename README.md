#Simple Keylogger [POC]
- - -
##Notice:
>This keylogger is considered a proof of concept for our [Ethical Hacking class final research project.](http://web.eng.fiu.edu/~aperezpo/EEL4789/index.htm)
>It is in **no way** meant to be used for malicious purposes.

##Project Structure:

* **bin** *(contains binary files for the project)*
* **src** *(contains the source code files for the project)*
* **src/client** *(contains the source code files for the client side)*
* **src/client/loader** *(contains the source code files for the client loader executable loader.exe)*
* **src/client/library** *(contains the source code files for the client keylogger library library.dll)*
* **src/server** *(contains the source code files for the server side)*

##How it Works:
Attacker first needs to setup a server (PHP) using the script files provided. Once the server is setup, the keylogger client files, specifically library.dll, need to be compiled with the right IP address for the remote server connection.

>#####Keylogging Process:
* Attacker executes **loader.exe** in the victim's computer and waits for it to inject/load library.dll to the firefox.exe process.
* Library.dll upon being loaded, will intercept the **GetMessage() API** calls within firefox.exe, and filter out key press messages.
* Library.dll begins logging key presses to a temporary file, and uploading the file to the attacker's server once it reaches the desired amount of key presses per log file (specified in the code).
* Library.dll is also attempting to provide the attacker with command line access to the client's computer by actively trying to open a connection to a listening application in a machine specified by the attacker.


>#####Server:
* The server side provides the attacker a single location to view the log files of all the victims.
* Listen on a port with netcat for a remote shell into the machine.

- - -

##TODO
#####The following are beyond the scope of the initial project:
1. Find a way to permanently install on the client's computer
2. Properly Bypass UAC
3. Encrypt communication between client and server
4. Add support for other browsers
5. Make library configuration file based
6. Support/Test on multiple Operating Systems
