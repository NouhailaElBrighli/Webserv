#include "ListeningSocket.hpp"

// Setters
void AN::ListeningSocket::set_listening(int listening) {
	this->listening = listening;
}

// Constructors and copy constructor and copy assignment operator and destructor
AN::ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int backlog) : BindingSocket(domain, service, protocol, port, interface), backlog(backlog) {
	// Start listening for connections
	set_listening(start_listening());
	test_connection(listening);
}

AN::ListeningSocket::ListeningSocket(const ListeningSocket &socket) : BindingSocket(socket), backlog(socket.backlog) {
}

AN::ListeningSocket &AN::ListeningSocket::operator=(const ListeningSocket &socket) {
	BindingSocket::operator=(socket);
	backlog = socket.backlog;
	return *this;
}

AN::ListeningSocket::~ListeningSocket() {
}

// Definition of the virtual function from the parent class
int AN::ListeningSocket::start_listening() {
	// Start listening for connections
	return listen(get_sock(), backlog);
}