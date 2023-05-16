#ifndef MAINSERVER_HPP
#define MAINSERVER_HPP

#include "WSNetworking.hpp"

class MainServer {

  private:
	ConfigFileParser *config_file_parser;
	int				  backlog;

  private:
	vector<ListeningSocket> listen_socket;
	map<int, MainClient *>	clients;
	vector<t_addrinfo *>	address;
	vector<int>				socket;
	int						accept_socket, max_socket;
	fd_set					current_sockets, ready_sockets;
	bool					launch_status;

  private:
	// Getters
	ListeningSocket			get_listen_socket(int index) const;
	vector<ListeningSocket> get_listen_socket() const;
	string					get_request(int client_socket, string key);

  private:
	// Methods
	void run_sockets();
	int	 right_port(int client_socket);
	int	 right_address(int fd_socket);
	void accepter(int fd_socket);
	void handle(int client_socket);
	void responder(int client_socket);

	void init();
	void destroy_client(int i);

  public:
	// Constructors and destructor
	MainServer(ConfigFileParser *config_file_parser, int backlog);
	~MainServer();

	// Methods
	void launch();
};

#endif	// MAINSERVER_HPP
