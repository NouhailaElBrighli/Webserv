#include "BindSocket.hpp"

// Constructors and destructor
BindSocket::BindSocket(const char *host, const char *port, const string &server_name)
	: Socket(host, port, server_name) {
	// Establish network connection
	this->bind_sock = bind(this->socket_listen, this->bind_address->ai_addr, this->bind_address->ai_addrlen);
	freeaddrinfo(this->bind_address);
	if (this->bind_sock == -1) {
		close(this->socket_listen);
		throw std::runtime_error(STR_RED("Error: Bind socket failed on " + this->host + ":" + this->port));
	}
}

BindSocket::~BindSocket() {}
