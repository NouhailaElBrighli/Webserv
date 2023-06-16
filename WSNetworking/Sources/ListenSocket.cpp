#include "ListenSocket.hpp"

// Constructors and destructor
ListenSocket::ListenSocket(const char *host, const char *port, int backlog)
	: BindSocket(host, port) {
	// Start listen for connections
	this->listen_sock = listen(this->socket_listen, backlog);
	if (this->listen_sock < 0)
		throw std::runtime_error(
			STR_RED("Error: Listen socket failed on " + this->host + ":" + this->port));
}

ListenSocket::~ListenSocket() {}
