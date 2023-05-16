#ifndef LSTENINGSOCKET_HPP
#define LSTENINGSOCKET_HPP

#include "BindingSocket.hpp"

class ListeningSocket : public BindingSocket {

  protected:
	int listening;

  public:
	// Constructors and destructor
	ListeningSocket(const char *host, const char *port, int backlog);
	virtual ~ListeningSocket();
};

#endif	// LSTENINGSOCKET_HPP