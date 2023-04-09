#include "Server.hpp"

// Getters
WSN::ListeningSocket *WSN::Server::get_listen_socket() const {
	return listen_socket;
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::Server::Server(int domain, int service, int protocol, int port, u_long interface, int backlog) : listen_socket(new ListeningSocket(domain, service, protocol, port, interface, backlog)) {
}

WSN::Server::Server(const Server &server) : listen_socket(server.listen_socket) {
}

WSN::Server &WSN::Server::operator=(const Server &server) {
	listen_socket = server.listen_socket;
	return *this;
}

WSN::Server::~Server() {
	delete listen_socket;
}
