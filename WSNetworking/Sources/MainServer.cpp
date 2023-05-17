#include "MainServer.hpp"

static string truncate(string str, size_t width) {
	if (str.length() > width)
		return str.substr(0, width - 1) + ".";
	return str;
}

static void print_str(string str, size_t width) {
	cout << std::left << std::setw(width) << truncate(str, width);
}

// Methods
void MainServer::print_info() {
	cout << C_CYAN << "-----------------------------------------------" << endl
		 << C_CYAN << "| " << C_YELLOW;
	print_str("Server Name", 11);
	cout << C_CYAN << " | " << C_GREEN;
	print_str("Host", 11);
	cout << C_CYAN << " | " << C_RED;
	print_str("Port", 6);
	cout << C_CYAN << " | " << C_PURPLE;
	print_str("Socket", 6);
	cout << C_CYAN << " |" << C_RES << endl;
	cout << C_CYAN << "-----------------------------------------------" << endl;
	for (size_t i = 0; i < this->socket.size(); i++) {
		cout << C_CYAN << "| " << C_YELLOW;
		print_str(this->config_file_parser->get_config_server_parser(i)
					  ->get_server_name(),
				  11);
		cout << C_CYAN << " | " << C_GREEN;
		print_str(
			this->config_file_parser->get_config_server_parser(i)->get_host(),
			11);
		cout << C_CYAN << " | " << C_RED;
		print_str(this->config_file_parser->get_config_server_parser(i)
					  ->get_port_str(),
				  6);
		cout << C_CYAN << " | " << C_PURPLE << this->socket[i];
		print_str("", 5);
		cout << C_CYAN << " |" << C_RES << endl;
		cout << C_CYAN << "-----------------------------------------------"
			 << endl;
	}
}

// Getters
ListeningSocket MainServer::get_listen_socket(int index) const {
	return listen_socket[index];
}

vector<ListeningSocket> MainServer::get_listen_socket() const {
	return listen_socket;
}

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
void MainServer::run_sockets() {
	// Create a listening socket for each port
	for (size_t i = 0;
		 i < config_file_parser->get_config_server_parser().size(); i++)
		listen_socket.push_back(ListeningSocket(
			config_file_parser->get_config_server_parser(i)->get_host().c_str(),
			config_file_parser->get_config_server_parser(i)
				->get_port_str()
				.c_str(),
			backlog));

	// fill the address and socket vectors
	for (size_t i = 0; i < get_listen_socket().size(); i++) {
		this->address.push_back(get_listen_socket(i).get_bind_address());
		this->socket.push_back(get_listen_socket(i).get_socket_listen());
	}
	this->print_info();
}

int MainServer::right_port(int client_socket) {
	int		  port;
	socklen_t addrlen;
	bool	  socket_found = false;
	size_t	  i;

	// Get the socket address structure for the client socket
	for (i = 0; i < this->address.size(); i++) {
		addrlen = sizeof(this->address[i]);
		if (getsockname(client_socket, (sockaddr *)&this->address[i], &addrlen)
			!= 0)
			continue;
		else {
			socket_found = true;
			break;
		}
	}

	if (socket_found == false)
		throw std::runtime_error(str_red("port not found"));

	char host[NI_MAXHOST];
	for (size_t i = 0; i < this->address.size(); i++) {
		addrlen	   = sizeof(this->address[i]);
		int result = getnameinfo((t_sockaddr *)&address[i], addrlen, host,
								 NI_MAXHOST, nullptr, 0, 0);
		if (result == 0) {
			std::cout << "Server name: " << host << std::endl;
		} else {
			std::cerr << "Failed to retrieve server name: "
					  << gai_strerror(result) << std::endl;
		}
	}

	for (size_t i = 0;
		 i < this->config_file_parser->get_config_server_parser().size(); i++) {
		for (size_t j = 0; j < this->address.size(); j++) {
			port = ntohs(((sockaddr_in *)&this->address[j])->sin_port);
			if (this->config_file_parser->get_config_server_parser(i)
					->get_port()
				== port)
				return i;
		}
	}
	throw std::runtime_error(str_red("port not found"));
	return -1;
}

int MainServer::right_address(int fd_socket) {
	for (size_t i = 0; i < this->socket.size(); i++) {
		if (this->socket[i] == fd_socket)
			return i;
	}
	throw std::runtime_error(str_red("socket not found"));
	return -1;
}

void MainServer::accepter(int fd_socket) {
	socklen_t addrlen;

	print_long_line("accepter");
	int raddr = this->right_address(fd_socket);
	addrlen	  = sizeof(this->address[raddr]);
	this->accept_socket
		= accept(fd_socket, (t_sockaddr *)&this->address[raddr], &addrlen);
	cout << "fd_socket : " << fd_socket << endl;
	cout << "accept_socket : " << this->accept_socket << endl;
}

void MainServer::handle(int client_socket) {
	int i;
	print_long_line("handle");

	if ((i = this->right_port(client_socket)) != -1) {
		MainClient *mainClient = new MainClient(
			client_socket,
			this->config_file_parser->get_config_server_parser(i));
		this->clients[client_socket] = mainClient;
		print_long_line("add new client");
		return;
	}
}

void MainServer::responder(int client_socket) {
	print_long_line("responder");

	if (this->clients[client_socket]->get_status() < 400) {
		string accurate = "HTTP/1.1 ";
		accurate += this->clients[client_socket]->get_msg_status();
		accurate += "\r\nContent-type: text/html\r\n\r\n";
		accurate += "Hello From Server\nYou are Host : ";
		accurate += this->get_request(client_socket, "Host") + "\r\n\r\n";
		send(client_socket, accurate.c_str(), accurate.length(), 0);
	} else {
		string error = "HTTP/1.1 ";
		error += this->clients[client_socket]->get_msg_status();
		error += "\r\n\r\n";
		send(client_socket, error.c_str(), error.length(), 0);
	}
	int i;
	if ((i = this->right_port(client_socket)) != -1) {
		Cgi cgi(this->clients[client_socket], this->config_file_parser->get_config_server_parser(i)->get_config_location_parser());
	
	try {
		cgi.just_print();
	} catch (const std::exception &e) {
		throw std::runtime_error(e.what());
	}    
	}   
}     

void MainServer::init() {
	FD_ZERO(&this->current_sockets);
	for (size_t i = 0; i < this->socket.size(); i++)
		FD_SET(this->socket[i], &this->current_sockets);

	this->max_socket
		= *std::max_element(this->socket.begin(), this->socket.end());
}

void MainServer::destroy_client(int i) {
	print_long_line("destroy client");
	// Destroy the client
	delete this->clients[i];
	this->clients.erase(i);
	cout << C_PURPLE << "current socket to be destroy: " << std::to_string(i)
		 << C_RES << endl;
	for (size_t j = 0; j < this->socket.size(); j++)
		if (this->socket[j] == i)
			return;
	FD_CLR(i, &this->current_sockets);
	close(i);
	cout << C_RED << "current socket destroyed: " << std::to_string(i) << C_RES
		 << endl;
}

void MainServer::launch() {
	if (this->launch_status)
		return;
	this->launch_status = true;
	this->run_sockets();
	this->init();
	while (true) {
		print_long_line("Waiting for connection...");

		// because select() will modify the set, we need to reset it each time
		this->ready_sockets = this->current_sockets;

		// select() will block until there is activity on one of the sockets
		if (select(this->max_socket + 1, &this->ready_sockets, NULL, NULL, NULL)
			< 0)
			throw std::runtime_error("select() failed");
		print_long_line("select");
		// check if the listening socket is ready
		for (int i = 1; i <= this->max_socket; i++) {
			if (FD_ISSET(i, &this->ready_sockets)) {
				if (std::find(this->socket.begin(), this->socket.end(), i)
					!= this->socket.end()) {
					try {
						this->accepter(i);
						if (this->accept_socket > this->max_socket)
							this->max_socket = this->accept_socket;
						if (this->accept_socket < 0)
							continue;
						FD_SET(this->accept_socket, &this->current_sockets);
					} catch (const std::exception &e) {
						cerr << e.what() << endl;
					}
				} else {
					try {
						// handle the client's request
						this->handle(i);
						this->responder(i);
					} catch (const std::exception &e) {
						cerr << e.what() << endl;
					}
					this->destroy_client(i);
				}
			}
		}
	}
}
