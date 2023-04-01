#include "Server.hpp"

// Getters
WS::ListeningSocket *WS::Server::get_socket() const {
	return socket;
}

// Constructors and copy constructor and copy assignment operator and destructor
WS::Server::Server(int domain, int service, int protocol, int port, u_long interface, int backlog) : socket(new ListeningSocket(domain, service, protocol, port, interface, backlog)) {
}

WS::Server::Server(const Server &server) : socket(server.socket) {
}

WS::Server &WS::Server::operator=(const Server &server) {
	socket = server.socket;
	return *this;
}

WS::Server::~Server() {
	delete socket;
}
