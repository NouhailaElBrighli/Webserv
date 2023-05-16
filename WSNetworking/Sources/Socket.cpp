#include "Socket.hpp"

// Getters
t_addrinfo *Socket::get_bind_address() const { return this->bind_address; }

int Socket::get_socket_listen() const { return this->socket_listen; }

// Constructors and destructor
Socket::Socket(const char *host, const char *port) {
	memset(&this->hints, 0, sizeof(this->hints));
	hints.ai_family	  = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags	  = AI_PASSIVE;

	getaddrinfo(host, port, &this->hints, &this->bind_address);
	// Establish socket
	this->socket_listen
		= socket(this->bind_address->ai_family, this->bind_address->ai_socktype,
				 this->bind_address->ai_protocol);
	if (this->socket_listen < 0)
		throw std::runtime_error(str_red("Socket socket failed."));

	int reuse  = 1;
	int resock = setsockopt(this->socket_listen, SOL_SOCKET, SO_REUSEADDR,
							&reuse, sizeof(reuse));
	if (resock < 0)
		throw std::runtime_error(str_red("Socket socket failed."));
}

Socket::~Socket() {}
