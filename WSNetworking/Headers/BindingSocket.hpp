#ifndef BINDINGSOCKET_HPP
#define BINDINGSOCKET_HPP

#include "Socket.hpp"

class BindingSocket : public Socket {

  protected:
	int binding;

  public:
	// Constructors and destructor
	BindingSocket(const char *host, const char *port);
	virtual ~BindingSocket();
};

#endif	// BINDINGSOCKET_HPP