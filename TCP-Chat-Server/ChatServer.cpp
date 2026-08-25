#include "ChatServer.h"
#include<iostream>
#include <ws2tcpip.h>
#include<thread>
using namespace std;

#pragma comment(lib,"Ws2_32.lib")

chatserver::chatserver() {
	serversocket = INVALID_SOCKET;
}

chatserver::~chatserver() {
	if (serversocket != INVALID_SOCKET)
		closesocket(serversocket);
	WSACleanup();
}

bool chatserver::start() {
	cout << "\nServer starting......\n";
	
	if (!initializewinsock())return false;

	if (!createsocket())return false;

	if (!Bind())return false;

	if (!Listen())return false;

	cout << "\nServer Initialization Successfull!\n";

	acceptclients();

	return true;
}

bool chatserver::initializewinsock() {

	WSADATA data;

	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		cout << "\nWinsock initialzation failed!\n";
		return false;
	}
	cout << "\nWinsock initialized\n";
	return true;
}

bool chatserver::createsocket() {
	
	serversocket = socket(AF_INET, SOCK_STREAM, 0);

	if (serversocket == INVALID_SOCKET) {
		cout << "\nSocket creation failed!\n";
		WSACleanup();
		return false;
	}
	cout << "\nSocket created!\n";
	return true;

}

bool chatserver::Bind() {
	
	sockaddr_in serveraddress;

	serveraddress.sin_family = AF_INET;
	serveraddress.sin_port = htons(PORT);
	serveraddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(serversocket, (sockaddr*)&serveraddress, sizeof(serveraddress)) == SOCKET_ERROR) {
		cout << "\nBind failed!\n";
		return false;
	}

	cout << "\nBind Successfull!\n";
	return true;

}

bool chatserver::Listen() {
	
	if (listen(serversocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "\nListen failed!\n";
		return false;
	}
	cout << "\nListening for connections.....\n";
	return true;
}


void chatserver::acceptclients() {

	while (true) {
		cout << "\nWaiting for a client!\n";

		sockaddr_in clientaddress;
		int clientsize = sizeof(clientaddress);

		SOCKET clientsocket = accept(serversocket, (sockaddr*)&clientaddress, &clientsize);

		if (clientsocket == INVALID_SOCKET) {
			cout << "\nAccept failed!\n";
			return;
		}

		cout << "\nClient connected!\n";

		{
			lock_guard<mutex> lock(clientsmutex);

			clients.emplace_back(clientsocket);
		}

		thread clientthread(&chatserver::handleclient, this, clientsocket);

		clientthread.detach();
	}
}


void chatserver::handleclient(SOCKET clientsocket) {
	
	char buffer[4097];

	const char* prompt = "Enter username: ";

	send(clientsocket, prompt, static_cast<int>(strlen(prompt)), 0);

	char usernamebuffer[256];

	int usernamebytes = recv(clientsocket, usernamebuffer, sizeof(usernamebuffer) - 1, 0);

	if (usernamebytes <= 0) {
		cout << "\nClient disconnected before entering username.\n";
		removeclient(clientsocket);
		closesocket(clientsocket);
		return;
	}

	usernamebuffer[usernamebytes] = '\0';

	string username(usernamebuffer);

	{
		lock_guard<mutex>lock(clientsmutex);

		for (auto& client : clients) {
			if (client.getsocket() == clientsocket) {
				client.setusername(username);
				break;
			}
		}
	}

	{
		lock_guard<mutex>lock(coutmutex);

		cout << "user connected: " << username <<endl;
	}

	

	while (true) {
		int bytereceived = recv(clientsocket, buffer, sizeof(buffer) - 1, 0);

		if (bytereceived <= 0) {
			{
				lock_guard<mutex> lock(coutmutex);

				cout << username
					<< " disconnected!"
					<< endl;
			}

			break;
		}

		buffer[bytereceived] = '\0';

		string message(buffer);

		{
			lock_guard<mutex> lock(coutmutex);

			cout << username
				<< ": "
				<< message << endl;
		}

		string formattedmessage =username + ": " + message;

		BroadcastMessage(formattedmessage, clientsocket);
	}
	
	removeclient(clientsocket);
	closesocket(clientsocket);

}


void chatserver::removeclient(SOCKET clientsocket) {
	
	size_t connectedclients;

	{
		lock_guard<mutex> lock(clientsmutex);

		for (auto it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->getsocket() == clientsocket)
			{
				clients.erase(it);
				break;
			}
		}

		connectedclients = clients.size();
	}

	{
		lock_guard<mutex> lock(coutmutex);

		cout << "Client removed. Connected clients: "
			<< connectedclients
			<< endl;
	}
}


void chatserver::BroadcastMessage(
    const string& message,
    SOCKET sendersocket){

	vector<SOCKET>clientsockets;

	{
		lock_guard<mutex> lock(clientsmutex);

		for (const auto& client : clients) {

			if (client.getsocket() != sendersocket) {
				clientsockets.push_back(client.getsocket());
			}
		}
	}

	for (SOCKET clientsocket : clientsockets) {

		int bytesent = send(
			clientsocket, message.c_str(), static_cast<int>(message.size()), 0);

		if (bytesent == SOCKET_ERROR) {

			lock_guard<mutex> lock(coutmutex);
			cout << "Failed to send message to client" << endl;
		}
	}


}

