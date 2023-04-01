#include "BindingSocket.hpp"

// Getters
int WS::BindingSocket::get_binding() const {
	return binding;
}

// Setters
void WS::BindingSocket::set_binding(int binding) {
	this->binding = binding;
}

// Constructors and copy constructor and copy assignment operator and destructor
WS::BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interface) : Socket(domain, service, protocol, port, interface) {
	// Establish network connection
	binding = connect_to_network(get_sock(), get_address());
	test_connection(binding, "Binding");
}

WS::BindingSocket::BindingSocket(const BindingSocket &socket) : Socket(socket), binding(socket.binding) {
}

WS::BindingSocket &WS::BindingSocket::operator=(const BindingSocket &socket) {
	Socket::operator=(socket);
	binding = socket.binding;
	return *this;
}

WS::BindingSocket::~BindingSocket() {
}

// Definition of the virtual function from the parent class
int WS::BindingSocket::connect_to_network(int sock, t_sockaddr_in address) {
	// Bind the socket to the address
	return bind(sock, (t_sockaddr *)&address, sizeof(address));
}