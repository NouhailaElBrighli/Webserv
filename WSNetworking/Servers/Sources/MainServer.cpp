#include "MainServer.hpp"

// Getters
WSN::ListeningSocket WSN::MainServer::get_listen_socket(int index) const {
	return listen_socket[index];
}

vector<WSN::ListeningSocket> WSN::MainServer::get_listen_socket() const {
	return listen_socket;
}

string WSN::MainServer::get_request(int client_socket, string key) {
	return this->clients[client_socket]->get_request(key);
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::MainServer::MainServer(int domain, int service, int protocol, vector<int> port, u_long interface, int backlog) {
	// Create a listening socket for each port
	for (size_t i = 0; i < port.size(); i++)
		listen_socket.push_back(ListeningSocket(domain, service, protocol, port[i], interface, backlog));

	this->address = get_listen_socket(0).get_address();

	// fill the socket vector with the socket of each listening socket
	for (size_t i = 0; i < get_listen_socket().size(); i++)
		this->socket.push_back(get_listen_socket(i).get_socket());

	this->launch();
}

WSN::MainServer::MainServer(const MainServer &main_server) : listen_socket(main_server.listen_socket), address(main_server.address), socket(main_server.socket) {
}

WSN::MainServer &WSN::MainServer::operator=(const MainServer &main_server) {
	listen_socket = main_server.listen_socket;
	this->address = main_server.address;
	this->socket  = main_server.socket;
	return *this;
}

WSN::MainServer::~MainServer() {
}

void WSN::MainServer::accepter(int accept_socket) {
	print_line("accepter");

	char	  client_address[MAXLINE + 1];
	socklen_t addrlen = sizeof(address);

	this->accept_socket = accept(accept_socket, (t_sockaddr *)&address, &addrlen);
	cout << "Accept socket : " << this->accept_socket << endl;

	inet_ntop(AF_INET, &address, client_address, MAXLINE);
	cout << "Client connection : " << client_address << endl;
}

void WSN::MainServer::handle(int client_socket) {
	print_line("handle");

	WSN::MainClient *mainClient	 = new WSN::MainClient(client_socket);
	this->clients[client_socket] = mainClient;
}

void WSN::MainServer::responder(int client_socket) {
	print_line("responder");

	if (this->clients[client_socket]->get_status() == true) {
		string hello = "HTTP/1.0 200OK\r\n\r\n";
		hello += "Hello From Server\nYou are Host : ";
		hello += this->get_request(client_socket, "Host") + "\n";
		// write(client_socket, hello, strlen(hello));
		send(client_socket, hello.c_str(), hello.length(), 0);
	} else {
		string error = "HTTP/1.0 ";
		error += this->clients[client_socket]->get_msg_status();
		error += "\r\n\r\n";
		// write(client_socket, error, strlen(error));
		send(client_socket, error.c_str(), error.length(), 0);
	}
}

void WSN::MainServer::init() {
	FD_ZERO(&this->current_sockets);
	for (size_t i = 0; i < this->socket.size(); i++)
		FD_SET(this->socket[i], &this->current_sockets);

	this->max_socket = *std::max_element(this->socket.begin(), this->socket.end());
}

void WSN::MainServer::destroy_client(int i) {
	// if (this->clients[i].get_request("Connection") != "keep-alive")
	FD_CLR(i, &this->current_sockets);
	// Destroy the client
	delete this->clients[i];
	this->clients.erase(i);
	close(i);
}

void WSN::MainServer::launch() {
	init();
	while (true) {
		print_line("Waiting for connection...");

		// because select() will modify the set, we need to reset it each time
		this->ready_sockets = this->current_sockets;

		// select() will block until there is activity on one of the sockets
		if (select(this->max_socket + 1, &this->ready_sockets, NULL, NULL, NULL) < 0)
			throw std::runtime_error("select() failed");

		// check if the listening socket is ready
		for (int i = 1; i <= this->max_socket; i++) {
			if (FD_ISSET(i, &this->ready_sockets)) {
				if (std::find(this->socket.begin(), this->socket.end(), i) != this->socket.end()) {

					accepter(i);
					if (this->accept_socket > this->max_socket)
						this->max_socket = this->accept_socket;
					if (this->accept_socket < 0)
						continue;
					FD_SET(this->accept_socket, &this->current_sockets);

				} else {
					try {
						// handle the client's request
						handle(i);
						responder(i);
					} catch (const std::exception &e) {
						cerr << e.what() << endl;
					}
					destroy_client(i);
				}
			}
		}
	}
}
