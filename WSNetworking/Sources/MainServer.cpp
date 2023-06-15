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
	// Initialize the server sockets
	this->init_server_sockets();
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
	cout << C_CYAN << " |" << endl;
	cout << string(65, '-') << endl;

	for (size_t i = 0; i < config_file_parser->get_config_server_parser().size(); i++) {
		cout << C_CYAN << "| " << C_BLUE;
		print_int(static_cast<int>(i) + 1, 6);
		cout << C_CYAN << "| " << C_YELLOW;
		print_str(config_file_parser->get_config_server_parser(i)->get_server_name(), 13);
		cout << C_CYAN << " | " << C_GREEN;
		print_str(config_file_parser->get_config_server_parser(i)->get_host(), 15);
		cout << C_CYAN << " | " << C_RED;
		print_str(config_file_parser->get_config_server_parser(i)->get_port_str(), 7);
		cout << C_CYAN << " | " << C_PURPLE;
		print_int(port_socket[config_file_parser->get_config_server_parser(i)->get_port()], 9);
		cout << C_CYAN << " |" << endl;
		cout << string(65, '-') << C_RES << endl;
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
			this->socket_server[this->get_listen_socket(i).get_socket_listen()]
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
		throw std::runtime_error(STR_RED("Error getting socket information"));

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

// Initialize the server sockets
void MainServer::init_server_sockets() {
	FD_ZERO(&this->read_sockets);
	std::memset(&this->read_sockets, 0, sizeof(this->read_sockets));

	FD_ZERO(&this->write_sockets);
	std::memset(&this->write_sockets, 0, sizeof(this->write_sockets));

	for (map<int, int>::iterator it = this->socket_server.begin(); it != this->socket_server.end();
		 it++)
		FD_SET((*it).second, &this->read_sockets);

	// max element of the socket map
	this->max_socket = this->socket_server.rbegin()->second;
}

// Reset read sockets
void MainServer::reset() {
	FD_ZERO(&this->read_sockets);
	std::memset(&this->read_sockets, 0, sizeof(this->read_sockets));

	FD_ZERO(&this->write_sockets);
	std::memset(&this->write_sockets, 0, sizeof(this->write_sockets));

	for (map<int, int>::iterator it = this->socket_server.begin(); it != this->socket_server.end();
		 it++)
		FD_SET((*it).second, &this->read_sockets);

	for (map<int, int>::iterator it = this->socket_client.begin(); it != this->socket_client.end();
		 it++) {
		if (this->clients[(*it).second]->get_phase() == READ_PHASE)
			FD_SET((*it).second, &this->read_sockets);
		else if (this->clients[(*it).second]->get_phase() == WRITE_PHASE)
			FD_SET((*it).second, &this->write_sockets);
	}
}

// Routine methods
void MainServer::accepter(int fd_socket) {
	t_sockaddr *address;
	socklen_t	addrlen = sizeof(address);

	this->accept_socket = accept(fd_socket, (t_sockaddr *)&address, &addrlen);
	if (this->accept_socket == -1)
		throw std::runtime_error(STR_RED("Error accept"));

	if (this->accept_socket > this->max_socket)
		this->max_socket = this->accept_socket;

	this->socket_client[this->accept_socket] = this->accept_socket;

	FD_SET(this->accept_socket, &this->read_sockets);
}

void MainServer::create_client(int client_socket) {
	int i;

	PRINT_LONG_LINE("create client");
	if ((i = this->right_server(client_socket)) != -1) {
		MainClient *mainClient
			= new MainClient(client_socket, this->config_file_parser->get_config_server_parser(i));
		this->clients[client_socket] = mainClient;
		return;
	} else if (i == -1) {
		MainClient *mainClient
			= new MainClient(client_socket, this->config_file_parser->get_config_server_parser(0));
		this->clients[client_socket] = mainClient;
		return;
	}
}

void MainServer::handle(int client_socket, string task) {
	PRINT_LONG_LINE("handle " + task);

	if (this->clients.find(client_socket) != this->clients.end())
		this->clients[client_socket]->start(task);

	else {
		this->create_client(client_socket);
		this->clients[client_socket]->start(task);
	}
}

void MainServer::destroy_client(int client_socket) {
	string			  show;
	std::stringstream sscs;

	sscs << client_socket;
	PRINT_LONG_LINE("destroy client");

	// Check if the client is a master socket
	show = string(C_YELLOW) + "current socket to be close: " + sscs.str();
	SHOW_INFO(show);
	if (this->clients.find(client_socket) != this->clients.end()
		&& this->clients[client_socket]->get_send_receive_status() == true) {
		show = string(C_RED) + "current client '" + sscs.str()
			   + "' can't be closed now, until the response is done.";
		SHOW_INFO(show);
		return;
	}
	if (this->socket_server.find(client_socket) != this->socket_server.end()) {
		show = string(C_RED) + "current socket '" + sscs.str()
			   + "' mustn't be close, because it's a master socket.";
		return;
	}
	// Destroy the client
	if (this->clients.find(client_socket) != this->clients.end())
		delete this->clients[client_socket];

	this->clients.erase(client_socket);
	this->socket_client.erase(client_socket);

	close(client_socket);
	show = string(C_RED) + "current socket closed: " + sscs.str();
	SHOW_INFO(show);
}

// Main routine
void MainServer::routine() {
	signal(SIGPIPE, SIG_IGN);
	while (true) {

		this->reset();

		PRINT_LONG_LINE("select wait for client");
		// select() will block until there is activity on one of the sockets
		if (select(this->max_socket + 1, &this->read_sockets, &this->write_sockets, NULL, NULL)
			== -1)
			throw std::runtime_error(STR_RED("Error select : ") + strerror(errno));

		// check if the listening socket is ready
		for (int i = 3; i <= this->max_socket; i++) {
			if (FD_ISSET(i, &this->read_sockets) || FD_ISSET(i, &this->write_sockets)) {
				// check if the socket is a master socket
				if (this->socket_server.find(i) != this->socket_server.end()) {
					try {
						this->accepter(i);
					} catch (const std::exception &e) {
						cerr << e.what() << endl;
					}
					continue;
				}
				try {
					// if the socket is ready for reading, call the handle_read function
					if (FD_ISSET(i, &this->read_sockets))
						this->handle(i, "read");

					// if the socket is ready for writing, call the handle_write function
					else if (FD_ISSET(i, &this->write_sockets))
						this->handle(i, "write");

				} catch (const std::exception &e) {
					this->clients[i]->set_send_receive_status(false);
					cerr << e.what() << endl;
				}
				this->destroy_client(i);
			}
		}
	}
}
