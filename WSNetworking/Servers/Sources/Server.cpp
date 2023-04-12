#include "Server.hpp"

// Getters
WSN::ListeningSocket WSN::Server::get_listen_socket(int index) const {
	return listen_socket[index];
}

vector<WSN::ListeningSocket> WSN::Server::get_listen_socket() const {
	return listen_socket;
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::Server::Server(int domain, int service, int protocol, vector<int> port, u_long interface, int backlog) {
	// Create a listening socket for each port
	for (size_t i = 0; i < port.size(); i++)
		listen_socket.push_back(ListeningSocket(domain, service, protocol, port[i], interface, backlog));
}

WSN::Server::Server(const Server &server) : listen_socket(server.listen_socket) {
}

WSN::Server &WSN::Server::operator=(const Server &server) {
	listen_socket = server.listen_socket;
	return *this;
}

WSN::Server::~Server() {
}
