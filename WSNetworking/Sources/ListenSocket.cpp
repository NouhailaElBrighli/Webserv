#include "ListenSocket.hpp"

// Constructors and destructor
ListenSocket::ListenSocket(const char *host, const char *port, const string &server_name, int backlog)
	: BindSocket(host, port, server_name) {
	// Start listen for connections
	this->listen_sock = listen(this->socket_listen, backlog);
	if (this->listen_sock < 0) {
		close(this->socket_listen);
		throw std::runtime_error(STR_RED("Error: Listen socket failed on " + this->host + ":" + this->port));
	}
}

ListenSocket::~ListenSocket() {}
