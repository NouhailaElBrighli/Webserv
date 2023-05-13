#include "MainServer.hpp"

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

// Constructors and copy constructor and copy assignment operator and destructor
MainServer::MainServer(int domain, int service, int protocol,
					   ConfigFileParser *config_file_parser, u_long interface,
					   int backlog)
	: config_file_parser(config_file_parser),

	  domain(domain), service(service), protocol(protocol),
	  interface(interface), backlog(backlog) {

	launch_status = false;
}

MainServer::MainServer(const MainServer &main_server)
	: listen_socket(main_server.listen_socket), address(main_server.address),
	  socket(main_server.socket) {}

MainServer &MainServer::operator=(const MainServer &main_server) {
	listen_socket = main_server.listen_socket;
	this->address = main_server.address;
	this->socket  = main_server.socket;
	return *this;
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
			domain, service, protocol,
			config_file_parser->get_config_server_parser(i)->get_port(),
			interface, backlog));

	this->address = get_listen_socket(0).get_address();

	// fill the socket vector with the socket of each listening socket
	for (size_t i = 0; i < get_listen_socket().size(); i++)
		this->socket.push_back(get_listen_socket(i).get_socket());

	// print port and socket
	for (size_t i = 0; i < this->socket.size(); i++)
		cout
			<< C_YELLOW << "Port : "
			<< this->config_file_parser->get_config_server_parser(i)->get_port()
			<< "	Socket : " << this->socket[i] << C_RES << endl;
}

void MainServer::accepter(int accept_socket) {
	print_line("accepter");

	char	  client_address[MAXLINE + 1];
	socklen_t addrlen = sizeof(address);

	this->accept_socket
		= accept(accept_socket, (t_sockaddr *)&address, &addrlen);
	cout << "this->accept_socket : " << this->accept_socket << endl;
	cout << "accept_socket : " << accept_socket << endl;

	inet_ntop(AF_INET, &address, client_address, MAXLINE);
	cout << "Client connection : " << client_address << endl;

	in_addr_t client_ip = ntohl(address.sin_addr.s_addr);
	cout << "Client IP : " << client_ip << endl;

	// Get the socket address structure for the client socket
	cout
		<< "getsockname(this->accept_socket, (sockaddr *)&address, &addrlen) : "
		<< getsockname(this->accept_socket, (sockaddr *)&address, &addrlen)
		<< endl;

	// Get the port number from the socket address structure
	uint16_t port = ntohs(((sockaddr_in *)&address)->sin_port);
	cout << "port : " << port << endl;
}

void MainServer::handle(int client_socket) {
	socklen_t addrlen = sizeof(address);

	print_line("handle");

	// Get the socket address structure for the client socket
	if (getsockname(this->accept_socket, (sockaddr *)&address, &addrlen) != 0)
		return;

	// Get the port number from the socket address structure
	uint16_t port = ntohs(((sockaddr_in *)&address)->sin_port);
	for (size_t i = 0; i < this->socket.size(); i++) {
		if (this->config_file_parser->get_config_server_parser(i)->get_port()
			== port) {
			MainClient *mainClient = new MainClient(
				client_socket,
				this->config_file_parser->get_config_server_parser(i));
			this->clients[client_socket] = mainClient;
			print_line("add new client");
			break;
		}
	}
}

void MainServer::responder(int client_socket) {
	print_line("responder");

	if (this->clients[client_socket]->get_status() < 400) {
		string accurate = "HTTP/1.1 ";
		accurate += this->clients[client_socket]->get_msg_status();
		accurate += "\r\n\r\n";
		accurate += "Hello From Server\nYou are Host : ";
		accurate += this->get_request(client_socket, "Host") + "\n";
		send(client_socket, accurate.c_str(), accurate.length(), 0);
	} else {
		string error = "HTTP/1.1 ";
		error += this->clients[client_socket]->get_msg_status();
		error += "\r\n\r\n";
		send(client_socket, error.c_str(), error.length(), 0);
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
	print_line("destroy client");
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
		print_line("Waiting for connection...");

		// because select() will modify the set, we need to reset it each time
		this->ready_sockets = this->current_sockets;

		// select() will block until there is activity on one of the sockets
		if (select(this->max_socket + 1, &this->ready_sockets, NULL, NULL, NULL)
			< 0)
			throw std::runtime_error("select() failed");

		// check if the listening socket is ready
		for (int i = 1; i <= this->max_socket; i++) {
			if (FD_ISSET(i, &this->ready_sockets)) {
				if (std::find(this->socket.begin(), this->socket.end(), i)
					!= this->socket.end()) {
					this->accepter(i);
					if (this->accept_socket > this->max_socket)
						this->max_socket = this->accept_socket;
					if (this->accept_socket < 0)
						continue;
					FD_SET(this->accept_socket, &this->current_sockets);
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
