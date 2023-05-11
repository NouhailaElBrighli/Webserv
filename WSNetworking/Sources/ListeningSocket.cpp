#include "ListeningSocket.hpp"

// Getters
int ListeningSocket::get_listening() const { return this->listening; }

// Setters
void ListeningSocket::set_listening(int listening) {
	this->listening = listening;
}

// Constructors and copy constructor and copy assignment operator and destructor
ListeningSocket::ListeningSocket(int domain, int service, int protocol,
								 int port, u_long interface, int backlog)
	: BindingSocket(domain, service, protocol, port, interface),
	  backlog(backlog) {
	// Start listening for connections
	set_listening(start_listening());
	test_connection(this->listening, "Listening");
}

ListeningSocket::ListeningSocket(const ListeningSocket &socket)
	: BindingSocket(socket), backlog(socket.backlog) {
	set_listening(socket.get_listening());
	this->backlog = socket.backlog;
}

ListeningSocket &ListeningSocket::operator=(const ListeningSocket &socket) {
	BindingSocket::operator=(socket);
	set_listening(socket.get_listening());
	this->backlog = socket.backlog;
	return *this;
}

ListeningSocket::~ListeningSocket() {}

// Definition of the virtual function from the parent class
int ListeningSocket::start_listening() {
	// Start listening for connections
	return listen(get_socket(), this->backlog);
}