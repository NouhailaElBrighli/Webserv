#include "MainServer.hpp"

// Constructors and copy constructor and copy assignment operator and destructor
WSN::MainServer::MainServer(int domain, int service, int protocol, vector<int> port, u_long interface, int backlog) : Server(domain, service, protocol, port, interface, backlog) {
	this->address = get_listen_socket(0).get_address();

	// fill the socket vector with the socket of each listening socket
	for (size_t i = 0; i < get_listen_socket().size(); i++)
		this->socket.push_back(get_listen_socket(i).get_socket());
	launch();
}

WSN::MainServer::MainServer(const MainServer &main_server) : Server(main_server), address(main_server.address), socket(main_server.socket) {
}

WSN::MainServer &WSN::MainServer::operator=(const MainServer &main_server) {
	Server::operator=(main_server);
	this->address = main_server.address;
	this->socket  = main_server.socket;
	return *this;
}

WSN::MainServer::~MainServer() {
}

void WSN::MainServer::accepter(int accept_socket) {
	print_line("accepter");
	char client_address[MAXLINE + 1];

	accept_socket = accept(this->socket[accept_socket], (t_sockaddr *)&this->address, (socklen_t *)&this->address);

	inet_ntop(AF_INET, &address, client_address, MAXLINE);
	cout << "Client connection : " << client_address << endl;
}

void WSN::MainServer::handle(int client_socket) {
	print_line("handle");
	// std::memset(buffer, 0, MAXLINE);
	// read(client_socket, buffer, MAXLINE);

	// string request(buffer);
	// cout << request << endl;
	// // cout << buffer;
	// try {
	// 	this->request_parser.run(request);
	// 	print_line("Request Parser");
	// 	cout << this->request_parser << endl;
	// } catch (const std::exception &e) {
	// 	print_line("Parse Error");
	// 	cout << e.what() << endl;
	// }

	try {
		WSN::MainClient client(client_socket);
		// this->clients[client_socket] = client;
	} catch (const std::exception &e) {
		cout << e.what() << endl;
	}
}

void WSN::MainServer::responder(int client_socket) {
	print_line("responder");
	string hello = "HTTP/1.0 200OK\r\n\r\nHello From Server";
	// write(client_socket, hello, strlen(hello));
	send(client_socket, hello.c_str(), hello.length(), 0);
	cout << "Hello message sent" << endl;
}

void WSN::MainServer::launch() {
	fd_set current_sockets, ready_sockets;
	int	   max_socket;

	FD_ZERO(&current_sockets);
	for (size_t i = 0; i < this->socket.size(); i++)
		FD_SET(this->socket[i], &current_sockets);
	// get the biggest number from the vector
	max_socket = *std::max_element(this->socket.begin(), this->socket.end());
	while (true) {
		print_line("Waiting for connection...");

		// because select() will modify the set, we need to reset it each time
		ready_sockets = current_sockets;

		// select() will block until there is activity on one of the sockets
		if (select(max_socket + 1, &ready_sockets, NULL, NULL, NULL) < 0)
			throw std::runtime_error("select() failed");

		// check if the listening socket is ready
		for (size_t i = 1; i <= max_socket; i++) {
			if (FD_ISSET(i, &ready_sockets)) {
				if (std::find(this->socket.begin(), this->socket.end(), i) != this->socket.end()) {
					// accept the new connection
					accepter(i);
					if (this->accept_socket > max_socket)
						max_socket = this->accept_socket;
					FD_SET(this->accept_socket, &current_sockets);
				} else {
					// handle the client's request
					handle(i);
					responder(i);
					// if (this->clients[i].get_request("Connection") != "keep-alive") {
					FD_CLR(i, &current_sockets);
					// Destroy the client
					print_line("Closing connection");
					// }
					clients.erase(i);
					print_line("Done");
				}
			}
		}
	}
}
