#include "Server.hpp"

// Getters
WSN::ListeningSocket *WSN::Server::get_socket() const {
	return socket;
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::Server::Server(int domain, int service, int protocol, int port, u_long interface, int backlog) : socket(new ListeningSocket(domain, service, protocol, port, interface, backlog)) {
}

WSN::Server::Server(const Server &server) : socket(server.socket) {
}

WSN::Server &WSN::Server::operator=(const Server &server) {
	socket = server.socket;
	return *this;
}

WSN::Server::~Server() {
	delete socket;
}
