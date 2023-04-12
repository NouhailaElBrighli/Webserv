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
	this->address.sin_port		  = htons(port);
	this->address.sin_addr.s_addr = htonl(interface);
	// Establish socket
	this->socket_v = socket(domain, service, protocol);
	cout << "this->socket_v = " << this->socket_v << endl;
	test_connection(this->socket_v, "Socket");
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
	close(this->socket_v);
}

void WSN::Socket::test_connection(int item_to_test, const string &name) {
	// Confirme that the socket or connection or binding or listening hes been properly established
	if (item_to_test < 0) {
		// std::cout << C_RED << "Test failed !" << C_RES << std::endl;
		// exit(EXIT_FAILURE);
		throw std::runtime_error(C_RED + name + " socket failed" + C_RES);
	}
}