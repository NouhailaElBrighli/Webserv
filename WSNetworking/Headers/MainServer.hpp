#ifndef MAINSERVER_HPP
#define MAINSERVER_HPP

#include "WSNetworking.hpp"

class MainServer {

  private:
	ConfigFileParser	   *config_file_parser;
	vector<ListeningSocket> listen_socket;
	map<int, MainClient *>	clients;
	t_sockaddr_in			address;
	vector<int>				socket;
	int						accept_socket, max_socket;
	fd_set					current_sockets, ready_sockets;
	bool					launch_status;

  private:
	int	   domain;
	int	   service;
	int	   protocol;
	u_long interface;
	int	   backlog;

  private:
	void run_sockets();
	void accepter(int accept_socket);
	void handle(int client_socket);
	void responder(int client_socket);

	void init();
	void destroy_client(int i);

  public:
	// Getters
	ListeningSocket			get_listen_socket(int index) const;
	vector<ListeningSocket> get_listen_socket() const;
	string					get_request(int client_socket, string key);

	// Constructors and copy constructor and copy assignment operator and
	// destructor
	MainServer(int domain, int service, int protocol,
			   ConfigFileParser *config_file_parser, u_long interface,
			   int backlog);
	MainServer(const MainServer &main_server);
	MainServer &operator=(const MainServer &main_server);
	~MainServer();

	// Methods
	void launch();
};

#endif	// MAINSERVER_HPP
