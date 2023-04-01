#include "ListeningSocket.hpp"

// Setters
void WS::ListeningSocket::set_listening(int listening) {
	this->listening = listening;
}

// Constructors and copy constructor and copy assignment operator and destructor
WS::ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int backlog) : BindingSocket(domain, service, protocol, port, interface), backlog(backlog) {
	// Start listening for connections
	set_listening(start_listening());
	test_connection(listening, "Listening");
}

WS::ListeningSocket::ListeningSocket(const ListeningSocket &socket) : BindingSocket(socket), backlog(socket.backlog) {
}

WS::ListeningSocket &WS::ListeningSocket::operator=(const ListeningSocket &socket) {
	BindingSocket::operator=(socket);
	backlog = socket.backlog;
	return *this;
}

WS::ListeningSocket::~ListeningSocket() {
}

// Definition of the virtual function from the parent class
int WS::ListeningSocket::start_listening() {
	// Start listening for connections
	return listen(get_sock(), backlog);
}