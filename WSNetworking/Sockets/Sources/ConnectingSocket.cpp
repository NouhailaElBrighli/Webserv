#include "ConnectingSocket.hpp"

// Getters
int WS::ConnectingSocket::get_connection() const {
	return connection;
}

// Setters
void WS::ConnectingSocket::set_connection(int connection) {
	this->connection = connection;
}
// Constructors and copy constructor and copy assignment operator and destructor
WS::ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long interface) : Socket(domain, service, protocol, port, interface) {
	// Establish network connection
	set_connection(connect_to_network(get_sock(), get_address()));
	test_connection(connection, "Connection");
}

WS::ConnectingSocket::ConnectingSocket(const ConnectingSocket &socket) : Socket(socket), connection(socket.connection) {
}

WS::ConnectingSocket &WS::ConnectingSocket::operator=(const ConnectingSocket &socket) {
	Socket::operator=(socket);
	connection = socket.connection;
	return *this;
}

WS::ConnectingSocket::~ConnectingSocket() {
	close(connection);
}

// Definition of the virtual function from the parent class
int WS::ConnectingSocket::connect_to_network(int sock, t_sockaddr_in address) {
	// Connect the socket to the address
	return connect(sock, (t_sockaddr *)&address, sizeof(address));
}
