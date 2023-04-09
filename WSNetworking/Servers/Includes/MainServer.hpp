#ifndef MAINSERVER_HPP
#define MAINSERVER_HPP

#include "Server.hpp"

namespace WSN {

class MainServer : public Server {

  private:
	t_sockaddr_in address;
	int			  socket;
	char		  buffer[MAXLINE + 1];
	int			  accept_socket;

	virtual void accepter();
	virtual void handle();
	virtual void responder();

  public:
	// Constructors and copy constructor and  copy assignment operator and destructor
	MainServer(int domain, int service, int protocol, int port, u_long interface, int backlog);
	MainServer(const MainServer &main_server);
	MainServer &operator=(const MainServer &main_server);
	virtual ~MainServer();

	virtual void launch();
};

} // namespace WSN

#endif // MAINSERVER_HPP