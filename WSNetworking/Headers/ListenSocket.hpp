#ifndef LISTENSOCKET_HPP
#define LISTENSOCKET_HPP

#include "BindSocket.hpp"

class ListenSocket : public BindSocket {

  protected:
	int listen_sock;

  public:
	// Constructors and destructor
	ListenSocket(const char *host, const char *port, const string &server_name, int backlog);
	virtual ~ListenSocket();
};

#endif	// LISTENSOCKET_HPP