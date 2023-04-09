#include "MainServer.hpp"

// Constructors and copy constructor and copy assignment operator and destructor
WSN::MainServer::MainServer(int domain, int service, int protocol, int port, u_long interface, int backlog) : Server(domain, service, protocol, port, interface, backlog) {
	this->address = get_listen_socket()->get_address();
	this->socket  = get_listen_socket()->get_socket();
	launch();
}

WSN::MainServer::MainServer(const MainServer &main_server) : Server(main_server) {
	this->address = main_server.address;
	this->socket  = main_server.socket;
}

WSN::MainServer &WSN::MainServer::operator=(const MainServer &main_server) {
	Server::operator=(main_server);
	this->address = main_server.address;
	this->socket  = main_server.socket;
	return *this;
}

WSN::MainServer::~MainServer() {
}

void WSN::MainServer::accepter() {
	char client_address[MAXLINE + 1];

	accept_socket = accept(this->socket, (t_sockaddr *)&this->address, (socklen_t *)&this->address);
	// int select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict errorfds, struct timeval *restrict timeout);
	// accept_socket = select(sock + 1, &buffer, NULL, NULL, NULL);

	inet_ntop(AF_INET, &address, client_address, MAXLINE);
	cout << "Client connection : " << client_address << endl;

	std::memset(buffer, 0, MAXLINE);
	read(accept_socket, buffer, MAXLINE);
}

void WSN::MainServer::handle() {
	string request(buffer);
	cout << request << endl;
	// cout << buffer;
	try {
		WSN::RequestParser requestParser(request);

		// for (map<string, string>::const_iterator it = requestParser.get_request().begin(); it != requestParser.get_request().end(); it++) {
		// 	cout << it->first << " : " << it->second << endl;
		// }
	} catch (const std::exception &e) {
		cout << "-------------------------------------------- ParseError: " << e.what() << endl;
	}
}

void WSN::MainServer::responder() {
	string hello = "HTTP/1.0 200OK\r\n\r\nHello From Server 123 ";
	// write(accept_socket, hello, strlen(hello));
	send(accept_socket, hello.c_str(), hello.length(), 0);
	cout << "Hello message sent" << endl;
	close(accept_socket);
}

void WSN::MainServer::launch() {
	// fd_set current_sockets, ready_sockets;

	// FD_ZERO(&current_sockets);
	// FD_SET(this->socket, &current_sockets);
	while (true) {
		cout << "-------------------------------------------- Waiting for connection..." << endl;
		cout << "-------------------------------------------- accepter" << endl;
		accepter();
		cout << "-------------------------------------------- handle" << endl;
		handle();
		cout << "-------------------------------------------- responder" << endl;
		responder();
		cout << "-------------------------------------------- Done" << endl;
	}
}
