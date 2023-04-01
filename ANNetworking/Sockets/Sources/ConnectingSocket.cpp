#include "ConnectingSocket.hpp"

// Getters
int AN::ConnectingSocket::get_connection() const {
	return connection;
}

// Setters
void AN::ConnectingSocket::set_connection(int connection) {
	this->connection = connection;
}
// Constructors and copy constructor and copy assignment operator and destructor
AN::ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long interface) : Socket(domain, service, protocol, port, interface) {
	// Establish network connection
	set_connection(connect_to_network(get_sock(), get_address()));
	test_connection(connection);
}

AN::ConnectingSocket::ConnectingSocket(const ConnectingSocket &socket) : Socket(socket), connection(socket.connection) {
}

AN::ConnectingSocket &AN::ConnectingSocket::operator=(const ConnectingSocket &socket) {
	Socket::operator=(socket);
	connection = socket.connection;
	return *this;
}

AN::ConnectingSocket::~ConnectingSocket() {
	close(connection);
}

// Definition of the virtual function from the parent class
int AN::ConnectingSocket::connect_to_network(int sock, t_sockaddr_in address) {
	// Connect the socket to the address
	return connect(sock, (t_sockaddr *)&address, sizeof(address));
}
