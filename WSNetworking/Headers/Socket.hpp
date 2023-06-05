#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Include.hpp"

class Socket {

  protected:
	string		port, host;
	t_addrinfo	hints;
	t_addrinfo *bind_address;
	int			socket_listen;

  public:
	// Getters
	t_addrinfo *get_bind_address() const;
	int			get_socket_listen() const;

	// Constructors and destructor
	Socket(const char *host, const char *port);
	virtual ~Socket();
};

#endif	// SOCKET_HPP