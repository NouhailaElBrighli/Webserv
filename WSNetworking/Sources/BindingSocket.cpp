#include "BindingSocket.hpp"

// Getters
int BindingSocket::get_binding() const {
	return this->binding;
}

// Setters
void BindingSocket::set_binding(int binding) {
	this->binding = binding;
}

// Constructors and copy constructor and copy assignment operator and destructor
BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interface) : Socket(domain, service, protocol, port, interface) {
	// Establish network connection
	this->binding = connect_to_network(get_socket(), get_address());
	test_connection(this->binding, "Binding");
}

BindingSocket::BindingSocket(const BindingSocket &socket) : Socket(socket), binding(socket.binding) {
	this->binding = socket.binding;
}

BindingSocket &BindingSocket::operator=(const BindingSocket &socket) {
	Socket::operator=(socket);
	this->binding = socket.binding;
	return *this;
}

BindingSocket::~BindingSocket() {
}

// Definition of the virtual function from the parent class
int BindingSocket::connect_to_network(int socket, t_sockaddr_in address) {
	// Bind the socket to the address
	return bind(socket, (t_sockaddr *)&address, sizeof(address));
}