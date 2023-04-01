#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>

#define C_RES "\033[0m"
#define C_RED "\033[1;31m"
#define C_GREEN "\033[1;32m"
#define C_CYAN "\033[1;36m"
#define C_Yellow "\033[1;33m"

typedef struct sockaddr_in t_sockaddr_in;
typedef struct sockaddr	   t_sockaddr;

namespace AN {

class Socket {

  private:
	t_sockaddr_in address;
	int			  sock;

  public:
	// Getters
	t_sockaddr_in get_address() const;
	int			  get_sock() const;

	// Constructors and copy constructor and copy assignment operator and destructor
	Socket(int domain, int service, int protocol, int port, u_long interface);
	Socket(const Socket &socket);
	Socket &operator=(const Socket &socket);
	virtual ~Socket();

	virtual int connect_to_network(int sock, t_sockaddr_in address) = 0;

	void test_connection(int item_to_test);
};
} // namespace AN

#endif // SOCKET_HPP