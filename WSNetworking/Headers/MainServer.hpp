#ifndef MAINSERVER_HPP
#define MAINSERVER_HPP

#include "WSNetworking.hpp"


class MainServer {

  private:
	vector<ListeningSocket> listen_socket;
	map<int, MainClient *>	clients;
	t_sockaddr_in			address;
	vector<int>				socket;
	int						accept_socket, max_socket;
	fd_set					current_sockets, ready_sockets;

	void accepter(int accept_socket);
	void handle(int client_socket);
	void responder(int client_socket);
	void launch();

	void init();
	void destroy_client(int i);

  public:
	// Getters
	ListeningSocket			get_listen_socket(int index) const;
	vector<ListeningSocket> get_listen_socket() const;
	string					get_request(int client_socket, string key);

	// Constructors and copy constructor and copy assignment operator and destructor
	MainServer(int domain, int service, int protocol, vector<int> port, u_long interface, int backlog);
	MainServer(const MainServer &main_server);
	MainServer &operator=(const MainServer &main_server);
	~MainServer();
};


#endif // MAINSERVER_HPP
