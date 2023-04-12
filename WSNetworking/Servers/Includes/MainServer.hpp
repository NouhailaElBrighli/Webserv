#ifndef MAINSERVER_HPP
#define MAINSERVER_HPP

#include "Server.hpp"

namespace WSN {

class MainServer : public Server {

  private:
	// map of all the clients
	map<int, MainClient> clients;
	// RequestParser		 request_parser;
	t_sockaddr_in address;
	vector<int>	  socket;
	int			  accept_socket;
	// char				 buffer[MAXLINE + 1];

	virtual void accepter(int accept_socket);
	virtual void handle(int client_socket);
	virtual void responder(int client_socket);

  public:
	// Constructors and copy constructor and  copy assignment operator and destructor
	MainServer(int domain, int service, int protocol, vector<int> port, u_long interface, int backlog);
	MainServer(const MainServer &main_server);
	MainServer &operator=(const MainServer &main_server);
	virtual ~MainServer();

	virtual void launch();
};

} // namespace WSN

#endif // MAINSERVER_HPP
