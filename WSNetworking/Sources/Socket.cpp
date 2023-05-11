#include "Socket.hpp"

// Getters
t_sockaddr_in Socket::get_address() const { return this->address; }

int Socket::get_socket() const { return this->socket_v; }

// Constructors and copy constructor and copy assignment operator and destructor
Socket::Socket(int domain, int service, int protocol, int port,
			   u_long interface) {
	this->address.sin_family = domain;
	// htons() converts the unsigned short integer hostshort from host byte
	// order to network byte order.
	this->address.sin_port = htons(port);
	// htonl() converts the unsigned long integer hostlong from host byte order
	// to network byte order.
	this->address.sin_addr.s_addr = htonl(interface);

	// Establish socket
	this->socket_v = socket(domain, service, protocol);
	test_connection(this->socket_v, "Socket");

	int reuse = 1;

	test_connection(setsockopt(this->socket_v, SOL_SOCKET, SO_REUSEADDR, &reuse,
							   sizeof(reuse)),
					"setsockopt");
}

Socket::Socket(const Socket &socket) {
	this->address  = socket.get_address();
	this->socket_v = socket.get_socket();
}

Socket &Socket::operator=(const Socket &socket) {
	this->address  = socket.get_address();
	this->socket_v = socket.get_socket();
	return *this;
}

Socket::~Socket() {}

void Socket::test_connection(int item_to_test, const string &name) {
	// Confirme that the socket or connection or binding or listening hes been
	// properly established
	if (item_to_test < 0)
		throw std::runtime_error(C_RED + name + " socket failed" + C_RES);
}