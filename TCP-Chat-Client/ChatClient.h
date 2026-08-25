#pragma once

#include <winsock2.h>
#include <string>

using namespace std;

class chatclient
{
public:
    chatclient();
    ~chatclient();

    bool connect();

    void sendmessage(const std::string& message);
    void receivemessages();

private:
    SOCKET clientsocket;
};