#include "MainServer.hpp"

// Static functions
static void print_str(string str, size_t width) {
	size_t strLength = str.length();
	if (strLength >= width) {
		cout << str.substr(0, width);
	} else {
		size_t padding = (width - strLength) / 2;
		cout << std::setw(padding) << "" << str << std::setw(width - padding - strLength) << "";
	}
}

static void print_int(int integer, size_t width) {
	std::ostringstream oss;
	oss << integer;
	std::string str = oss.str();

	int padding		 = width - str.length();
	int leftPadding	 = padding / 2;
	int rightPadding = padding - leftPadding;

	std::cout << std::setw(leftPadding) << "" << integer << std::setw(rightPadding) << "";
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
	cout << C_CYAN << string(65, '-') << endl;
	cout << C_CYAN << "| " << C_BLUE;
	print_str("N°", 7);
	cout << C_CYAN << "| " << C_YELLOW;
	print_str("Server Name", 13);
	cout << C_CYAN << " | " << C_GREEN;
	print_str("Host", 15);
	cout << C_CYAN << " | " << C_RED;
	print_str("Port", 7);
	cout << C_CYAN << " | " << C_PURPLE;
	print_str("FD Socket", 9);
	cout << C_CYAN << " |" << C_RES << endl;
	cout << C_CYAN << string(65, '-') << endl;

	for (size_t i = 0; i < this->config_file_parser->get_config_server_parser().size(); i++) {
		cout << C_CYAN << "| " << C_BLUE;
		print_int(static_cast<int>(i) + 1, 6);
		cout << C_CYAN << "| " << C_YELLOW;
		print_str(this->config_file_parser->get_config_server_parser(i)->get_server_name(), 13);
		cout << C_CYAN << " | " << C_GREEN;
		print_str(this->config_file_parser->get_config_server_parser(i)->get_host(), 15);
		cout << C_CYAN << " | " << C_RED;
		print_str(this->config_file_parser->get_config_server_parser(i)->get_port_str(), 7);
		cout << C_CYAN << " | " << C_PURPLE;
		print_int(
			this->port_socket[this->config_file_parser->get_config_server_parser(i)->get_port()],
			9);
		cout << C_CYAN << " |" << C_RES << endl;
		cout << C_CYAN << string(65, '-') << endl;
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

	// Extract the port number
	port = right_port(client_socket);
	// Check if the port is in the config file and get the index
	for (size_t i = 0; i < this->config_file_parser->get_config_server_parser().size(); i++) {
		if (this->config_file_parser->get_config_server_parser(i)->get_port() == port)
			return i;
	}
	return -1;
}

// Initialize the reading sockets
void MainServer::init_reading_sockets() {
	FD_ZERO(&this->read_sockets);
	std::memset(&this->read_sockets, 0, sizeof(this->read_sockets));

	FD_ZERO(&this->write_sockets);
	std::memset(&this->write_sockets, 0, sizeof(this->write_sockets));

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

	FD_ZERO(&this->write_sockets);
	std::memset(&this->write_sockets, 0, sizeof(this->write_sockets));

	this->read_sockets	= this->current_sockets;
	this->write_sockets = this->current_sockets;
}

// Routine methods
void MainServer::accepter(int fd_socket) {
	t_sockaddr *address;
	socklen_t	addrlen = sizeof(address);

	this->accept_socket = accept(fd_socket, (t_sockaddr *)&address, &addrlen);
	if (this->accept_socket == -1)
		throw std::runtime_error(str_red("Error accept"));
}

void MainServer::create_client(int client_socket, string task) {
	int i;

	print_long_line("create client");
	if ((i = this->right_server(client_socket)) != -1) {
		MainClient *mainClient = new MainClient(
			client_socket, this->config_file_parser->get_config_server_parser(i), task);
		this->clients[client_socket] = mainClient;
		return;
	} else if (i == -1) {
		MainClient *mainClient = new MainClient(
			client_socket, this->config_file_parser->get_config_server_parser(0), task);
		this->clients[client_socket] = mainClient;
		return;
	}
}

void MainServer::handle_read(int client_socket) {
	print_long_line("handle_read");

	if (this->clients.find(client_socket) != this->clients.end())
		this->clients[client_socket]->start("read");
	else
		this->create_client(client_socket, "read");
}

void MainServer::handle_write(int client_socket) {
	print_long_line("handle_write");

	if (this->clients.find(client_socket) != this->clients.end())
		this->clients[client_socket]->start("write");
	else
		this->create_client(client_socket, "write");
}

void MainServer::destroy_client(int client_socket) {
	print_long_line("destroy client");
	// Check if the client is a master socket
	cout << C_YELLOW << "current socket to be close: " << client_socket << C_RES << endl;
	if (this->clients[client_socket]->get_send_receive_status() == true) {
		cout << C_RED << "current client '" << client_socket
			 << "' can't be closed now, until the response is done." << C_RES << endl;
		return;
	}
	if (this->socket[client_socket] == client_socket) {
		cout << C_GREEN << "current socket '" << client_socket
			 << "' mustn't be close, because it's a master socket." << C_RES << endl;
		return;
	}
	// Destroy the client
	if (this->clients.find(client_socket) != this->clients.end())
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
		if (select(this->max_socket + 1, &this->read_sockets, &this->write_sockets, NULL, NULL)
			== -1)
			throw std::runtime_error(str_red("Error select : ") + strerror(errno));

		// check if the listening socket is ready
		for (int i = 3; i <= this->max_socket; i++) {
			if (FD_ISSET(i, &this->read_sockets) || FD_ISSET(i, &this->write_sockets)) {
				// check if the socket is a master socket
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
						// if the socket is ready for reading, call the handle_read function
						if (FD_ISSET(i, &this->read_sockets))
							this->handle_read(i);
						// if the socket is ready for writing, call the handle_write function
						else if (FD_ISSET(i, &this->write_sockets))
							this->handle_write(i);

						this->destroy_client(i);
					} catch (const std::exception &e) {
						cerr << e.what() << endl;
					}
				}
			}
		}
	}
}
