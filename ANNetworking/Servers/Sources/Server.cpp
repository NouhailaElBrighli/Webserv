#include "Server.hpp"

// Getters
AN::ListeningSocket *AN::Server::get_socket() const {
	return socket;
}

// Constructors and copy constructor and copy assignment operator and destructor
AN::Server::Server(int domain, int service, int protocol, int port, u_long interface, int backlog) : socket(new ListeningSocket(domain, service, protocol, port, interface, backlog)) {
}

AN::Server::Server(const Server &server) : socket(server.socket) {
}

AN::Server &AN::Server::operator=(const Server &server) {
	socket = server.socket;
	return *this;
}

AN::Server::~Server() {
	delete socket;
}
