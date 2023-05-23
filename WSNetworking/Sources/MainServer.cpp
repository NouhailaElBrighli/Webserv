#include "MainServer.hpp"

// Static functions
static string truncate(string str, size_t width) {
	if (str.length() > width)
		return str.substr(0, width - 1) + ".";
	return str;
}

static void print_str(string str, size_t width) {
	cout << std::left << std::setw(width) << truncate(str, width);
}

static void print_int(int integer, size_t width) {
	cout << std::left << std::setw(width) << integer;
}

// Getters
ListenSocket MainServer::get_listen_socket(int index) const { return listen_socket[index]; }

vector<ListenSocket> MainServer::get_listen_socket() const { return listen_socket; }

string MainServer::get_request(int client_socket, string key) {
	return this->clients[client_socket]->get_request(key);
}

// Constructors and destructor
MainServer::MainServer(ConfigFileParser *config_file_parser, int backlog)
	: config_file_parser(config_file_parser), backlog(backlog) {
	launch_status = false;
}

MainServer::~MainServer() {
	for (size_t i = 0; i < this->clients.size(); i++)
		destroy_client(i);
}

// Methods
void MainServer::launch() {
	if (this->launch_status)
		return;
	this->launch_status = true;
	// Create a listening socket for each port
	this->run_sockets();
	// Print the server information
	this->print_info();
	// Initialize the reading sockets
	this->init_reading_sockets();
	// Run the server
	this->routine();
}

// Print the server information
void MainServer::print_info() {
	cout << C_CYAN << "-----------------------------------------------" << endl;
	cout << C_CYAN << "| " << C_YELLOW;
	print_str("Server Name", 11);
	cout << C_CYAN << " | " << C_GREEN;
	print_str("Host", 11);
	cout << C_CYAN << " | " << C_RED;
	print_str("Port", 6);
	cout << C_CYAN << " | " << C_PURPLE;
	print_str("Socket", 6);
	cout << C_CYAN << " |" << C_RES << endl;
	cout << C_CYAN << "-----------------------------------------------" << endl;

	for (size_t i = 0; i < this->config_file_parser->get_config_server_parser().size(); i++) {
		cout << C_CYAN << "| " << C_YELLOW;
		print_str(this->config_file_parser->get_config_server_parser(i)->get_server_name(), 11);
		cout << C_CYAN << " | " << C_GREEN;
		print_str(this->config_file_parser->get_config_server_parser(i)->get_host(), 11);
		cout << C_CYAN << " | " << C_RED;
		print_str(this->config_file_parser->get_config_server_parser(i)->get_port_str(), 6);
		cout << C_CYAN << " | " << C_PURPLE;
		print_int(
			this->port_socket[this->config_file_parser->get_config_server_parser(i)->get_port()],
			6);
		cout << C_CYAN << " |" << C_RES << endl;
		cout << C_CYAN << "-----------------------------------------------" << C_RES << endl;
	}
}

// Create sockets for each port
void MainServer::run_sockets() {
	// Create a listening socket for each port
	size_t j = 0;
	for (size_t i = 0; i < config_file_parser->get_config_server_parser().size(); i++) {
		try {
			listen_socket.push_back(ListenSocket(
				config_file_parser->get_config_server_parser(i)->get_host().c_str(),
				config_file_parser->get_config_server_parser(i)->get_port_str().c_str(), backlog));
			this->port_socket[config_file_parser->get_config_server_parser(i)->get_port()]
				= listen_socket[j].get_socket_listen();
			j++;
		} catch (const std::exception &e) {
			if (this->port_socket[config_file_parser->get_config_server_parser(i)->get_port()] == 0)
				throw std::runtime_error(e.what());
		}

		// fill the address and socket maps
		for (size_t i = 0; i < this->get_listen_socket().size(); i++)
			this->socket[this->get_listen_socket(i).get_socket_listen()]
				= this->get_listen_socket(i).get_socket_listen();
	}
}

// Tools for matching socket with server of config file
int MainServer::right_port(int client_socket) {
	socklen_t	  addr_len;
	t_sockaddr_in addr;

	addr_len = sizeof(addr);

	// Get the socket address structure for the client socket
	if (getsockname(client_socket, (struct sockaddr *)&addr, &addr_len) == -1)
		throw std::runtime_error(str_red("Error getting socket information"));

	// Extract the port number
	return (ntohs(addr.sin_port));
}

int MainServer::right_server(int client_socket) {
	int port;
	int first_server  = 0;
	int mutiple_ports = 0;

	// Extract the port number
	port = right_port(client_socket);
	// Check if the port is in the config file and get the index
	for (size_t i = 0; i < this->config_file_parser->get_config_server_parser().size(); i++) {
		if (this->config_file_parser->get_config_server_parser(i)->get_port() == port) {
			mutiple_ports++;
			first_server = i;
		}
	}
	if (mutiple_ports == 1)
		return first_server;
	else if (mutiple_ports > 1)
		throw std::runtime_error(str_cyan("Multiple ports found"));
	else if (mutiple_ports == 0)
		throw std::runtime_error(str_red("Error port not found"));
	return -1;
}

// Initialize the reading sockets
void MainServer::init_reading_sockets() {
	FD_ZERO(&this->read_sockets);
	std::memset(&this->read_sockets, 0, sizeof(this->read_sockets));

	FD_ZERO(&this->current_sockets);
	std::memset(&this->current_sockets, 0, sizeof(this->current_sockets));
	for (map<int, int>::iterator it = this->socket.begin(); it != this->socket.end(); it++)
		FD_SET((*it).second, &this->current_sockets);
	// max element of the socket map
	this->max_socket = this->socket.rbegin()->second;
}

// Reset read sockets
void MainServer::reset() {
	// because `select` will modify the set, we need to reset it each time
	FD_ZERO(&this->read_sockets);
	std::memset(&this->read_sockets, 0, sizeof(this->read_sockets));
	this->read_sockets = this->current_sockets;
}

// Routine methods
void MainServer::accepter(int fd_socket) {
	t_sockaddr *address;
	socklen_t	addrlen = sizeof(address);

	this->accept_socket = accept(fd_socket, (t_sockaddr *)&address, &addrlen);
	if (this->accept_socket == -1)
		throw std::runtime_error(str_red("Error accept"));
}

void MainServer::create_client(int client_socket) {
	int i;

	print_long_line("create client");
	try {
		if ((i = this->right_server(client_socket)) != -1) {
			MainClient *mainClient = new MainClient(
				client_socket, this->config_file_parser->get_config_server_parser(i),
				this->port_socket[this->right_port(client_socket)], true);
			this->clients[client_socket] = mainClient;
			return;
		}
	} catch (const std::exception &e) {
		if (string(e.what()).find("Multiple")) {
			MainClient *mainClient
				= new MainClient(client_socket, this->config_file_parser,
								 this->port_socket[this->right_port(client_socket)], false);
			this->clients[client_socket] = mainClient;
			return;
		} else
			throw std::runtime_error(e.what());
	}
}

void MainServer::handler(int client_socket) {
	print_long_line("handler");

	if (this->clients.find(client_socket) != this->clients.end()) {
		this->clients[client_socket]->start_handle();
	} else
		this->create_client(client_socket);
}

void MainServer::destroy_client(int client_socket) {
	print_long_line("destroy client");
	// Check if the client is a master socket
	cout << C_YELLOW << "current socket to be close: " << client_socket << C_RES << endl;
	if (this->clients[client_socket]->get_send_receive_status() == true) {
		cout << C_GREEN << "current client '" << client_socket
			 << "' mustn't be destroy, because it's still sending or recieving data." << C_RES
			 << endl;
		return;
	}
	if (this->socket[client_socket] == client_socket) {
		cout << C_GREEN << "current socket '" << client_socket
			 << "' mustn't be close, because it's a master socket." << C_RES << endl;
		return;
	}
	// Destroy the client
	delete this->clients[client_socket];
	this->clients.erase(client_socket);
	FD_CLR(client_socket, &this->current_sockets);
	close(client_socket);
	cout << C_RED << "current socket closed: " << client_socket << C_RES << endl;
}

// Main routine
void MainServer::routine() {
	while (true) {
		this->reset();

		print_long_line("select wait for client");
		// select() will block until there is activity on one of the sockets
		if (select(this->max_socket + 1, &this->read_sockets, NULL, NULL, NULL) == -1)
			throw std::runtime_error(str_red("Error select : ") + strerror(errno));

		// check if the listening socket is ready
		for (int i = 3; i <= this->max_socket; i++) {
			if (FD_ISSET(i, &this->read_sockets)) {
				if (i == this->socket[i]) {
					try {
						this->accepter(i);
						if (this->accept_socket > this->max_socket)
							this->max_socket = this->accept_socket;
						FD_SET(this->accept_socket, &this->current_sockets);
					} catch (const std::exception &e) {
						cerr << e.what() << endl;
					}
				} else {
					try {
						// handle the client's request
						this->handler(i);
					} catch (const std::exception &e) {
						cerr << e.what() << endl;
					}
					this->destroy_client(i);
				}
			}
		}
	}
}
