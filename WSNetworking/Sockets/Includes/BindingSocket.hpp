#ifndef BINDINGSOCKET_HPP
#define BINDINGSOCKET_HPP

#include "Socket.hpp"

namespace WS {

class BindingSocket : public Socket {

  private:
	int binding;

  public:
	// Getters
	int get_binding() const;

	// Setters
	void set_binding(int binding);

	// Constructors and copy constructor and copy assignment operator and destructor
	BindingSocket(int domain, int service, int protocol, int port, u_long interface);
	BindingSocket(const BindingSocket &socket);
	BindingSocket &operator=(const BindingSocket &socket);
	~BindingSocket();

	// Virtual function from perent class
	int connect_to_network(int sock, t_sockaddr_in address);
};
} // namespace WS

#endif // BINDINGSOCKET_HPP