#ifndef MAINSERVER_HPP
#define MAINSERVER_HPP

#include "WSNetworking.hpp"

class MainServer {

  private:
	ConfigFileParser *config_file_parser;
	int				  backlog;

  private:
	vector<ListenSocket>   listen_socket;
	map<int, MainClient *> clients;
	map<int, int>		   socket;
	int					   accept_socket, max_socket;
	fd_set				   current_sockets, read_sockets, write_sockets;
	bool				   launch_status;

  private:
	// Attributes for print_info()
	map<int, int> port_socket;

  private:
	// Getters
	ListenSocket		 get_listen_socket(int index) const;
	vector<ListenSocket> get_listen_socket() const;
	string				 get_request(int client_socket, string key);

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
	void reset();
	void accepter(int fd_socket);
	void create_client(int client_socket, string task);
	void handle_read(int client_socket);
	void handle_write(int client_socket);
	void destroy_client(int client_socket);

	// Main routine
	void routine();
};

#endif	// MAINSERVER_HPP
