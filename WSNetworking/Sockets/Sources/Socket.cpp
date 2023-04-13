#include "Socket.hpp"

// Getters
t_sockaddr_in WSN::Socket::get_address() const {
	return this->address;
}

int WSN::Socket::get_socket() const {
	return this->socket_v;
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::Socket::Socket(int domain, int service, int protocol, int port, u_long interface) {
	this->address.sin_family	  = domain;
	this->address.sin_port		  = htons(port);	  // htons() converts the unsigned short integer hostshort from host byte order to network byte order.
	this->address.sin_addr.s_addr = htonl(interface); // htonl() converts the unsigned long integer hostlong from host byte order to network byte order.

	// Establish socket
	this->socket_v = socket(domain, service, protocol);
	test_connection(this->socket_v, "Socket");

	int reuse = 1;

	test_connection(setsockopt(this->socket_v, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)), "setsockopt");
}

WSN::Socket::Socket(const Socket &socket) {
	this->address  = socket.get_address();
	this->socket_v = socket.get_socket();
}

WSN::Socket &WSN::Socket::operator=(const Socket &socket) {
	this->address  = socket.get_address();
	this->socket_v = socket.get_socket();
	return *this;
}

WSN::Socket::~Socket() {
}

void WSN::Socket::test_connection(int item_to_test, const string &name) {
	// Confirme that the socket or connection or binding or listening hes been properly established
	if (item_to_test < 0) {
		// std::cout << C_RED << "Test failed !" << C_RES << std::endl;
		// exit(EXIT_FAILURE);
		throw std::runtime_error(C_RED + name + " socket failed" + C_RES);
	}
}