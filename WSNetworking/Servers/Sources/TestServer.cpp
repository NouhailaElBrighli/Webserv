#include "TestServer.hpp"

// Constructors and copy constructor and copy assignment operator and destructor
WSN::TestServer::TestServer(int domain, int service, int protocol, int port, u_long interface, int backlog) : Server(domain, service, protocol, port, interface, backlog) {
	launch();
}

WSN::TestServer::TestServer(const TestServer &test_server) : Server(test_server) {
}

WSN::TestServer &WSN::TestServer::operator=(const TestServer &test_server) {
	Server::operator=(test_server);
	return *this;
}

WSN::TestServer::~TestServer() {
}

void WSN::TestServer::accepter() {
	t_sockaddr_in address = get_socket()->get_address();
	int			  sock	  = get_socket()->get_sock();

	new_socket = accept(sock, (t_sockaddr *)&address, (socklen_t *)&address);
	// int select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict errorfds, struct timeval *restrict timeout);
	// new_socket = select(sock + 1, &buffer, NULL, NULL, NULL);
	read(new_socket, buffer, FILENAME_MAX);
}

void WSN::TestServer::handle() {
	string request(buffer);
	cout << request << endl;
	// cout << buffer;
	try {
		WSN::RequestParser requestParser(request);

		for (map<string, string>::const_iterator it = requestParser.get_request().begin(); it != requestParser.get_request().end(); it++) {
			cout << it->first << " : " << it->second << endl;
		}
	} catch (const std::exception &e) {
		cout << "-------------------------------------------- ParseError: " << e.what() << endl;
	}
}

void WSN::TestServer::responder() {
	char hello[18] = "Hello from server";
	write(new_socket, hello, strlen(hello));
	// send(new_socket, hello, strlen(hello), 0);
	cout << "Hello message sent" << endl;
	close(new_socket);
}

void WSN::TestServer::launch() {
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