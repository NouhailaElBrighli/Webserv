#include "BindingSocket.hpp"

// Constructors and destructor
BindingSocket::BindingSocket(const char *host, const char *port)
	: Socket(host, port) {
	// Establish network connection
	this->binding = bind(this->socket_listen, this->bind_address->ai_addr,
						 this->bind_address->ai_addrlen);
	freeaddrinfo(this->bind_address);
	if (this->binding == -1)
		throw std::runtime_error(str_red("Binding socket failed"));
}

BindingSocket::~BindingSocket() {}
