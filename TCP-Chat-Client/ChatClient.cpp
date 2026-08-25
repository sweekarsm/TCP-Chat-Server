#include "ChatClient.h"

#include <iostream>
#include <ws2tcpip.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

chatclient::chatclient()
{
    clientsocket = INVALID_SOCKET;
}

chatclient::~chatclient()
{
    if (clientsocket != INVALID_SOCKET)
    {
        closesocket(clientsocket);
    }

    WSACleanup();
}

bool chatclient::connect()
{
    std::cout << "Client starting...\n";

    WSADATA data;

    if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
    {
        std::cout << "WinSock initialization failed.\n";
        return false;
    }

    clientsocket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (clientsocket == INVALID_SOCKET)
    {
        std::cout << "Socket creation failed.\n";
        return false;
    }

    sockaddr_in serveraddress{};

    serveraddress.sin_family = AF_INET;
    serveraddress.sin_port = htons(54000);

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &serveraddress.sin_addr
    );

    if (::connect(
        clientsocket,
        (sockaddr*)&serveraddress,
        static_cast<int>(sizeof(serveraddress))
    ) == SOCKET_ERROR)
    {
        std::cout << "Connection failed.\n";
        return false;
    }

    std::cout << "Connected to server!\n";

    return true;
}

void chatclient::sendmessage(const std::string& message)
{
    int bytessent = send(
        clientsocket,
        message.c_str(),
        static_cast<int>(message.size()),
        0
    );

    if (bytessent == SOCKET_ERROR)
    {
        std::cout << "Failed to send message.\n";
        return;
    }

    
}

string chatclient::receivemessage() {
    char buffer[4097];

    int bytesreceived = recv(clientsocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesreceived <= 0) {
        return "";
    }

    buffer[bytesreceived] = '\0';
    return string(buffer);
}

void chatclient::receivemessages() {
    
    char buffer[4097];

    while (true) {

        int bytesreceived = recv(clientsocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesreceived <= 0) {
            cout<<"\nDisconnected from server!\n";
            break;
        }

        buffer[bytesreceived] = '\0';

        cout << "\r" <<buffer << endl;
        cout << "You:" << flush;
    }

    
}


