#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Header.hpp"

namespace WSN {

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

	void test_connection(int item_to_test, const string &name);
};
} // namespace WSN

#endif // SOCKET_HPP