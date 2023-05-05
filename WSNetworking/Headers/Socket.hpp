#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Include.hpp"


class Socket {

  private:
	t_sockaddr_in address;
	int			  socket_v;

  public:
	// Getters
	t_sockaddr_in get_address() const;
	int			  get_socket() const;

	// Constructors and copy constructor and copy assignment operator and destructor
	Socket(int domain, int service, int protocol, int port, u_long interface);
	Socket(const Socket &socket);
	Socket &operator=(const Socket &socket);
	virtual ~Socket();

	virtual int connect_to_network(int socket, t_sockaddr_in address) = 0;

	void test_connection(int item_to_test, const string &name);
};

#endif // SOCKET_HPP