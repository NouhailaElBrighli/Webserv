#include "ListeningSocket.hpp"

// Constructors and destructor
ListeningSocket::ListeningSocket(const char *host, const char *port,
								 int backlog)
	: BindingSocket(host, port) {
	// Start listening for connections
	this->listening = listen(this->socket_listen, backlog);
	if (this->listening < 0)
		throw std::runtime_error(str_red("Listening socket failed"));
}

ListeningSocket::~ListeningSocket() {}
