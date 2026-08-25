#include "ClientSession.h"

clientsession::clientsession(SOCKET socket) :clientsocket(socket), username("Anonymous") {

}

SOCKET clientsession::getsocket() const {
	return clientsocket;
}

string clientsession::getusername() const {
	return username;
}

void clientsession::setusername(const::string& newusername) {
	username = newusername;
}