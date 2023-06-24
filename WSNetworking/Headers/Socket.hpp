#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Include.hpp"

class Socket {

  protected:
	string		port, host, server_name;
	t_addrinfo	hints;
	t_addrinfo *bind_address;
	int			socket_listen;

  public:
	// Getters
	t_addrinfo *get_bind_address() const;
	int			get_socket_listen() const;

	// Constructors and destructor
	Socket(const char *host, const char *port, const string &server_name);
	virtual ~Socket();
};

#endif	// SOCKET_HPP