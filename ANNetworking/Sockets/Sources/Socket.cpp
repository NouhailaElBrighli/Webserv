#include "Socket.hpp"

// Getters
t_sockaddr_in AN::Socket::get_address() const {
	return address;
}

int AN::Socket::get_sock() const {
	return sock;
}

// Constructors and copy constructor and copy assignment operator and destructor
AN::Socket::Socket(int domain, int service, int protocol, int port, u_long interface) {
	address.sin_family		= domain;
	address.sin_port		= htons(port);
	address.sin_addr.s_addr = htonl(interface);
	// Establish socket
	sock = socket(domain, service, protocol);
	test_connection(sock);
}

AN::Socket::Socket(const Socket &socket) {
	address = socket.get_address();
	sock	= socket.get_sock();
}

AN::Socket &AN::Socket::operator=(const Socket &socket) {
	address = socket.get_address();
	sock	= socket.get_sock();
	return *this;
}

AN::Socket::~Socket() {
	close(sock);
}

void AN::Socket::test_connection(int item_to_test) {
	// Confirme that the socket or connection hes been properly established
	if (item_to_test < 0) {
		std::cout << C_RED << "Test failed !" << C_RES << std::endl;
		exit(EXIT_FAILURE);
	}
}