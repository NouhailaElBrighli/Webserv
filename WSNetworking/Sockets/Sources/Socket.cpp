#include "Socket.hpp"

// Getters
t_sockaddr_in WSN::Socket::get_address() const {
	return address;
}

int WSN::Socket::get_sock() const {
	return sock;
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::Socket::Socket(int domain, int service, int protocol, int port, u_long interface) {
	address.sin_family		= domain;
	address.sin_port		= htons(port);
	address.sin_addr.s_addr = htonl(interface);
	// Establish socket
	sock = socket(domain, service, protocol);
	test_connection(sock, "Socket");
}

WSN::Socket::Socket(const Socket &socket) {
	address = socket.get_address();
	sock	= socket.get_sock();
}

WSN::Socket &WSN::Socket::operator=(const Socket &socket) {
	address = socket.get_address();
	sock	= socket.get_sock();
	return *this;
}

WSN::Socket::~Socket() {
	close(sock);
}

void WSN::Socket::test_connection(int item_to_test, const std::string &name) {
	// Confirme that the socket or connection or binding or listening hes been properly established
	if (item_to_test < 0) {
		// std::cout << C_RED << "Test failed !" << C_RES << std::endl;
		// exit(EXIT_FAILURE);
		throw std::runtime_error(C_RED + name + " socket failed" + C_RES);
	}
}