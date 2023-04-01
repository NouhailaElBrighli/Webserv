#ifndef LSTENINGSOCKET_HPP
#define LSTENINGSOCKET_HPP

#include "BindingSocket.hpp"

namespace WSN {

class ListeningSocket : public BindingSocket {

  private:
	int backlog;
	int listening;

  public:
	// Setters
	void set_listening(int listening);

	// Constructors and copy constructor and copy assignment operator and destructor
	ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int backlog);
	ListeningSocket(const ListeningSocket &socket);
	ListeningSocket &operator=(const ListeningSocket &socket);
	~ListeningSocket();

	// Virtual function from perent class
	int start_listening();
};
} // namespace WSN

#endif // LSTENINGSOCKET_HPP