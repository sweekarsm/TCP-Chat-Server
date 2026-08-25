#pragma once

#include<winsock2.h>
#include "ClientSession.h"
#include<vector>
#include<mutex>
#include<string>
using namespace std;

class chatserver {
public:
	chatserver();
	~chatserver();
	bool start();

private:
	bool initializewinsock();
	bool createsocket();
	bool Bind();
	bool Listen();
	void acceptclients();
	void handleclient(SOCKET clientsocket);
	void removeclient(SOCKET clientsocket);

	void BroadcastMessage(
		const string& message,
		SOCKET sendersocket);

	static constexpr unsigned short PORT = 54000;
	SOCKET serversocket;

	vector<clientsession> clients;
	mutex clientsmutex; 
};
