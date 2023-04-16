#ifndef MAINSERVER_HPP
#define MAINSERVER_HPP

#include "Server.hpp"

namespace WSN {

class MainServer : public Server {

  private:
	// map of all the clients
	map<int, MainClient *> clients;
	t_sockaddr_in		   address;
	vector<int>			   socket;
	int					   accept_socket, max_socket;
	fd_set				   current_sockets, ready_sockets;

	// Getters
	string get_request(int client_socket, string key);

	virtual void accepter(int accept_socket);
	virtual void handle(int client_socket);
	virtual void responder(int client_socket);

	void init();
	void destroy_client(int i);

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
