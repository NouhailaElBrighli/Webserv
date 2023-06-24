#include "Socket.hpp"

// Getters
t_addrinfo *Socket::get_bind_address() const { return this->bind_address; }

int Socket::get_socket_listen() const { return this->socket_listen; }

// Constructors and destructor
Socket::Socket(const char *host, const char *port, const string &server_name)
	: port(port), host(host), server_name(server_name) {
	memset(&this->hints, 0, sizeof(this->hints));
	hints.ai_family	  = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags	  = AI_PASSIVE;

	getaddrinfo(host, port, &this->hints, &this->bind_address);
	// Establish socket
	this->socket_listen
		= socket(this->bind_address->ai_family, this->bind_address->ai_socktype, this->bind_address->ai_protocol);
	if (this->socket_listen < 0)
		throw std::runtime_error(STR_RED("Error: Socket fd failed on " + this->host + ":" + this->port));

	// Set server name
	int set_name = setsockopt(this->socket_listen, SOL_SOCKET, SO_REUSEADDR, this->server_name.c_str(),
							  sizeof(this->server_name));
	if (set_name < 0) {
		close(this->socket_listen);
		throw std::runtime_error(STR_RED("Error: Set server name failed on " + this->host + ":" + this->port));
	}

	int on	  = 1;
	int reuse = setsockopt(this->socket_listen, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (reuse < 0) {
		close(this->socket_listen);
		throw std::runtime_error(STR_RED("Error: Socket setsockopt failed on " + this->host + ":" + this->port));
	}
#ifdef __APPLE__
	int nosig = setsockopt(this->socket_listen, SOL_SOCKET, SO_NOSIGPIPE, (char *)&on, sizeof(on));
	if (nosig < 0) {
		close(this->socket_listen);
		throw std::runtime_error(STR_RED("Error: Socket setsockopt failed on " + this->host + ":" + this->port));
	}
#endif
}

Socket::~Socket() {}
