#pragma once

#include<winsock2.h>
#include<string>
using namespace std;

class clientsession {
public:
	explicit clientsession(SOCKET socket);

	SOCKET getsocket() const;

	string getusername() const;
	void setusername(const string& username);

private:
	SOCKET clientsocket;
	string username;
};