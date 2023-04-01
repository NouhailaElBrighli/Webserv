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
	struct sockaddr_in address = get_socket()->get_address();
	int				   sock	   = get_socket()->get_sock();

	new_socket = accept(sock, (struct sockaddr *)&address, (socklen_t *)&address);
	// new_socket = select();
	read(new_socket, buffer, 1024);
}

void WSN::TestServer::handle() {
	cout << buffer << endl;
	cout << "--------------------------------------------" << endl;
}

void WSN::TestServer::respoder() {
	char hello[18] = "Hello from server";
	write(new_socket, hello, strlen(hello));
	// send(new_socket, hello, strlen(hello), 0);
	cout << "Hello message sent" << endl;
	close(new_socket);
}

void WSN::TestServer::launch() {
	while (true) {
		cout << "Waiting for connection..." << endl;
		accepter();
		handle();
		respoder();
		cout << "Done" << endl;
	}
}