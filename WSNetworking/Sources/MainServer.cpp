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

string MainServer::get_request(int client_socket, string key) { return this->clients[client_socket]->get_request(key); }

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
	PRINT_LONG_LINE("select wait for client");
}

// Create sockets for each port
void MainServer::run_sockets() {
	// Create a listening socket for each port
	size_t j = 0;
	for (size_t i = 0; i < config_file_parser->get_config_server_parser().size(); i++) {
		try {
			listen_socket.push_back(
				ListenSocket(config_file_parser->get_config_server_parser(i)->get_host().c_str(),
							 config_file_parser->get_config_server_parser(i)->get_port_str().c_str(),
							 config_file_parser->get_config_server_parser(i)->get_server_name(), backlog));
			this->port_socket[config_file_parser->get_config_server_parser(i)->get_port()]
				= listen_socket[j].get_socket_listen();
			j++;
		} catch (const std::exception &e) {
			throw std::runtime_error(e.what());
		}

		// fill the address and socket maps
		for (size_t i = 0; i < this->get_listen_socket().size(); i++)
			this->socket_server.push_back(this->get_listen_socket(i).get_socket_listen());
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
	return 0;
}

// Initialize the server sockets
void MainServer::init_server_sockets() {
	FD_ZERO(&this->read_sockets);
	std::memset(&this->read_sockets, 0, sizeof(this->read_sockets));

	FD_ZERO(&this->write_sockets);
	std::memset(&this->write_sockets, 0, sizeof(this->write_sockets));

	for (vector<int>::iterator it = this->socket_server.begin(); it != this->socket_server.end(); it++)
		FD_SET(static_cast<size_t>(*it), &this->read_sockets);

	// max element of the socket vector
	if (this->socket_server.size() > 0)
		this->max_socket = *std::max_element(this->socket_server.begin(), this->socket_server.end());
	else
		this->max_socket = 0;
}

// Reset read sockets
void MainServer::reset() {
	FD_ZERO(&this->read_sockets);
	std::memset(&this->read_sockets, 0, sizeof(this->read_sockets));

	FD_ZERO(&this->write_sockets);
	std::memset(&this->write_sockets, 0, sizeof(this->write_sockets));

	for (vector<int>::iterator it = this->socket_server.begin(); it != this->socket_server.end(); it++)
		FD_SET(static_cast<size_t>(*it), &this->read_sockets);

	if (this->socket_client.size() > 0) {
		for (vector<int>::iterator it = this->socket_client.begin(); it != this->socket_client.end(); it++) {
			if (this->clients[*it]->get_phase() == READ_PHASE)
				FD_SET(static_cast<size_t>(*it), &this->read_sockets);
			else if (this->clients[*it]->get_phase() == WRITE_PHASE)
				FD_SET(static_cast<size_t>(*it), &this->write_sockets);
		}
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

	this->socket_client.push_back(this->accept_socket);

	FD_SET(static_cast<size_t>(this->accept_socket), &this->read_sockets);
}

void MainServer::create_client(int client_socket) {
	// int i;

	PRINT_LONG_LINE("create client");
	MainClient *mainClient		 = new MainClient(client_socket, this->config_file_parser->get_config_server_parser(),
												  right_port(client_socket), right_server(client_socket));
	this->clients[client_socket] = mainClient;
	return;
}

void MainServer::handle(int client_socket, string task) {
	std::stringstream sscs;

	sscs << client_socket;
	PRINT_LONG_LINE("handle " + task + " for client with socket '" + sscs.str() + "'");

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

	show = string(C_YELLOW) + "client with socket '" + sscs.str() + "' to be close.";
	SHOW_INFO(show);
	if (this->clients.find(client_socket) != this->clients.end()
		&& this->clients[client_socket]->get_send_receive_status() == true) {
		show = string(C_CYAN) + "client with socket '" + sscs.str() + "' can't be closed now, until ";
		if (this->clients[client_socket]->get_phase() == READ_PHASE)
			show += string(C_RED) + "READ && RESPONSE" + string(C_CYAN);
		else if (this->clients[client_socket]->get_phase() == WRITE_PHASE)
			show += string(C_RED) + "RESPONSE" + string(C_CYAN);
		show += " is done.";
		SHOW_INFO(show);
		return;
	}

	// Check if the client is a main socket
	std::vector<int>::iterator server_it
		= std::find(this->socket_server.begin(), this->socket_server.end(), client_socket);
	if (server_it != this->socket_server.end()) {
		show = string(C_PURPLE) + "client with socket '" + sscs.str()
			   + "' mustn't be close, because it's a MAIN socket.";
		return;
	}

	// Destroy the client
	if (this->clients.find(client_socket) != this->clients.end())
		delete this->clients[client_socket];

	this->clients.erase(client_socket);
	std::vector<int>::iterator client_it
		= std::find(this->socket_client.begin(), this->socket_client.end(), client_socket);
	if (client_it != this->socket_client.end())
		this->socket_client.erase(client_it);
	close(client_socket);
	show = string(C_RED) + "client with socket '" + sscs.str() + "' closed.";
	SHOW_INFO(show);
}

// Main routine
void MainServer::routine() {
	// Set timeout to 10 seconds
	struct timeval timeout;
	timeout.tv_sec	= 10;
	timeout.tv_usec = 0;
	while (true) {

		this->reset();

		// select() will block until there is activity on one of the sockets
		if (select(this->max_socket + 1, &this->read_sockets, &this->write_sockets, NULL, &timeout) == -1)
			throw std::runtime_error(STR_RED("Error select : ") + strerror(errno));
		// check if the listening socket is ready
		for (int i = 3; i <= this->max_socket; i++) {
			if (FD_ISSET(i, &this->read_sockets) || FD_ISSET(i, &this->write_sockets)) {
				// check if the socket is a master socket
				vector<int>::iterator server_it = std::find(this->socket_server.begin(), this->socket_server.end(), i);
				if (server_it != this->socket_server.end()) {
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

				PRINT_LONG_LINE("select wait for client");
			}
		}
	}
}
