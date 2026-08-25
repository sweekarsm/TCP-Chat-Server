#include "ChatClient.h"

#include <iostream>
#include <string>
#include<thread>

using namespace std;

int main()
{
    chatclient client;

    if (!client.connect())
    {
        return 1;
    }

    thread receiver(&chatclient::receivemessages, &client);

    receiver.detach();

    string message;


    while (true)
    {
        cout << "You: ";
        getline(cin, message);

        if (message == "exit")
        {
            break;
        }

        client.sendmessage(message);
    }

    return 0;
}