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
	map<int, int>			socket;
	int						accept_socket, max_socket;
	fd_set					current_sockets, read_sockets;
	bool					launch_status;

  private:
	// Attributes for print_info()
	map<int, int> port_socket;

  private:
	// Getters
	ListeningSocket			get_listen_socket(int index) const;
	vector<ListeningSocket> get_listen_socket() const;
	string					get_request(int client_socket, string key);

  public:
	// Constructors and destructor
	MainServer(ConfigFileParser *config_file_parser, int backlog);
	~MainServer();

	// Methods
	void launch();

  private:
	// Print the server information
	void print_info();

	// Create sockets for each port
	void run_sockets();

	// Tools for matching socket with server of config file
	int right_port(int client_socket);
	int right_server(int client_socket);

	// Initialize the reading sockets
	void init_reading_sockets();

	// Routine methods
	void accepter(int fd_socket);
	void handle(int client_socket);
	void responder(int client_socket);
	void destroy_client(int i);

	// Main routine
	void routine();
};

#endif	// MAINSERVER_HPP
