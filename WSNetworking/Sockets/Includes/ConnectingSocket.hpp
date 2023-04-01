#ifndef CONNECTINGSOCKET_HPP
#define CONNECTINGSOCKET_HPP

#include "Socket.hpp"

namespace WSN {

class ConnectingSocket : public Socket {

  private:
	int connection;

  public:
	// Getters
	int get_connection() const;

	// Setters
	void set_connection(int connection);

	// Constructors and copy constructor and copy assignment operator and destructor
	ConnectingSocket(int domain, int service, int protocol, int port, u_long interface);
	ConnectingSocket(const ConnectingSocket &socket);
	ConnectingSocket &operator=(const ConnectingSocket &socket);
	~ConnectingSocket();

	// Virtual function from perent class
	int connect_to_network(int sock, t_sockaddr_in address);
};
} // namespace WSN

#endif // CONNECTINGSOCKET_HPP