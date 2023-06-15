#ifndef BINDSOCKET_HPP
#define BINDSOCKET_HPP

#include "Socket.hpp"

class BindSocket : public Socket {

  protected:
	int bind_sock;

  public:
	// Constructors and destructor
	BindSocket(const char *host, const char *port);
	virtual ~BindSocket();
};

#endif	// BINDSOCKET_HPP