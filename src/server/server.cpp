//server.cpp
//Note: This uses linux sockets so must
//be compiled and run on linux.

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
//#include "NetworkData.h"
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

void Listen();
void* SocketHandler(void* lp);
void DownloadLog(int *localcsock);
void TestMsg(int* lp);
void TestMsgBack(int *lp);

pthread_t thread_id=0;

int main(int argv, char** argc)
{
    Listen();
    return 0;
}

void Listen()
{
    int host_port= 1337;
    struct sockaddr_in my_addr;
    int hsock;
    sockaddr_in sadr;
    socklen_t addr_size = 0;
    int * p_int ;
    int err;
    int* csock;

    hsock = socket(AF_INET, SOCK_STREAM, 0);

    if(hsock == -1)
    {
        printf("Error initializing listening socket %d\n", errno);
        goto FINISH;
    }

    p_int = (int*)malloc(sizeof(int));
    *p_int = 1;

    if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
            (setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) )
    {
        printf("Error setting options for listening socket%d\n", errno);
        free(p_int);
        goto FINISH;
    }

    free(p_int);

    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons(host_port);
    memset(&(my_addr.sin_zero), 0, 8);
    my_addr.sin_addr.s_addr = INADDR_ANY ;

    //Setting up listening socket
    if( bind( hsock, (sockaddr*)&my_addr, sizeof(my_addr)) == -1 )
    {
        fprintf(stderr,"Error binding to socket, make sure nothing else is listening on this port %d\n",errno);
        goto FINISH;
    }
    if(listen( hsock, 10) == -1 )
    {
        fprintf(stderr, "Error listening %d\n",errno);
        goto FINISH;
    }

    addr_size = sizeof(sockaddr_in);

    while(true)
    {
        printf("Listening for a connection\n");
        csock = (int*)malloc(sizeof(int));
        if((*csock = accept( hsock, (sockaddr*)&sadr, &addr_size))!= -1)
        {
            printf("\n%s connected.\n",inet_ntoa(sadr.sin_addr));
            pthread_create(&thread_id,0,&SocketHandler, (void*)csock );
            pthread_detach(thread_id);
        }
        else
        {
            fprintf(stderr, "Error accepting %d\n", errno);
        }
    }

FINISH:
    free(csock);

}

void* SocketHandler(void* lp)
{

    //COMMANDS:
    string command_test = "test"; //Sends a test string
    string command_downloadFile = "getlog"; //Calls DownloadLog(int *localcsock)
    string command_testback = "testback";

    int *csock = (int*)lp;
    string input;


    while(1)
    {

        input = "";
        cout << "[Remote Terminal~]$ ";
        cin >> input;



        if(input == command_test)
        {
            TestMsg(csock);
        }
        else if(input == command_downloadFile)
        {
            DownloadLog(csock);
        }
        else if(input == "testback")
        {
            TestMsgBack(csock);
        }
        else
        {
            cout << "Invalid Command.\n";
        }
    }

FINISH:
    free(csock);
    return 0;
}

void DownloadLog(int *lp)
{
    //TODO: fix size of buffer to handle any size
    cout << "Getting Log...\n";
    char *sendBuffer = "getlog\n";
    int len;
    ssize_t bytesSent;
    len = strlen(sendBuffer);
    bytesSent = send(*lp, sendBuffer, 1024, 0);
    cout << "Log request sent.\n";
    char buffer[1024];
    int readEOF;
    ofstream logFile("log.txt");
    if((readEOF = read(*lp, buffer, 1024)))
    {
        printf("Log.txt has been updated\n");
    }
    else cout << "Error getting log\n";
    string bufferString(buffer);
    // logFile.open();
    logFile << bufferString;
    logFile.close();
}

void TestMsg(int *lp)
{
    cout << "Sennding test message...\n";
    char *sendBuffer = "testmsg\n";
    int len;
    ssize_t bytesSent;

    len = strlen(sendBuffer);
    bytesSent = send(*lp, sendBuffer, 1024, 0);
    //close(*lp);
    cout << "Test message sent.\n";
    /*
        ssize_t bytes_received;
        char recvBuffer[1024];
        bytes_received = recv(*lp, recvBuffer, 1024, 0);
        string bufferString(recvBuffer);
        cout << bufferString << "\n";
        */
}

void TestMsgBack(int *lp)
{
    cout << "Sennding test message...\n";
    char *sendBuffer = "testback";
    int len;
    ssize_t bytesSent;

    len = strlen(sendBuffer);
    bytesSent = send(*lp, sendBuffer, 1024, 0);
    //close(*lp);
    cout << "Test message sent.\n";
    /*
        ssize_t bytes_received;
        char recvBuffer[1024];
        bytes_received = recv(*lp, recvBuffer, 1024, 0);
        string bufferString(recvBuffer);
        cout << bufferString << "\n";
        */
}
