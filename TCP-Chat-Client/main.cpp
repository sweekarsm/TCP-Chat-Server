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

    string prompt = client.receivemessage();

    if (prompt.empty()) {
        cout << "\nServer disconnected.\n";
        return 1;
    }

    cout << prompt;

    string username;

    getline(cin, username);

    client.sendmessage(username);

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