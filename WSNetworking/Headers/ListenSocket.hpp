#ifndef LSTENSOCKET_HPP
#define LSTENSOCKET_HPP

#include "BindSocket.hpp"

class ListenSocket : public BindSocket {

  protected:
	int listen_sock;

  public:
	// Constructors and destructor
	ListenSocket(const char *host, const char *port, int backlog);
	virtual ~ListenSocket();
};

#endif	// LSTENSOCKET_HPP