#ifndef BINDSOCKET_HPP
#define BINDSOCKET_HPP

#include "Socket.hpp"

class BindSocket : public Socket {

  protected:
	int bind_sock;

  public:
	// Constructors and destructor
	BindSocket(const char *host, const char *port, const string &server_name);
	virtual ~BindSocket();
};

#endif	// BINDSOCKET_HPP