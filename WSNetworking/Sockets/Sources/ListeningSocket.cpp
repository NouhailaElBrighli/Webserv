#include "ListeningSocket.hpp"

// Getters
int WSN::ListeningSocket::get_listening() const {
	return this->listening;
}

// Setters
void WSN::ListeningSocket::set_listening(int listening) {
	this->listening = listening;
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int backlog) : BindingSocket(domain, service, protocol, port, interface), backlog(backlog) {
	// Start listening for connections
	set_listening(start_listening());
	test_connection(this->listening, "Listening");
}

WSN::ListeningSocket::ListeningSocket(const ListeningSocket &socket) : BindingSocket(socket), backlog(socket.backlog) {
}

WSN::ListeningSocket &WSN::ListeningSocket::operator=(const ListeningSocket &socket) {
	BindingSocket::operator=(socket);
	this->backlog = socket.backlog;
	return *this;
}

WSN::ListeningSocket::~ListeningSocket() {
}

// Definition of the virtual function from the parent class
int WSN::ListeningSocket::start_listening() {
	// Start listening for connections
	return listen(get_socket(), this->backlog);
}