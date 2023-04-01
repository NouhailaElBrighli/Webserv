#include "TestServer.hpp"

// Constructors and copy constructor and copy assignment operator and destructor
AN::TestServer::TestServer(int domain, int service, int protocol, int port, u_long interface, int backlog) : Server(domain, service, protocol, port, interface, backlog) {
	launch();
}

AN::TestServer::TestServer(const TestServer &test_server) : Server(test_server) {
}

AN::TestServer &AN::TestServer::operator=(const TestServer &test_server) {
	Server::operator=(test_server);
	return *this;
}

AN::TestServer::~TestServer() {
}

void AN::TestServer::accepter() {
	struct sockaddr_in address = get_socket()->get_address();
	int				   sock	   = get_socket()->get_sock();

	// new_socket = accept(sock, (struct sockaddr *)&address, (socklen_t *)&address);
	new_socket = select();
	read(new_socket, buffer, 3000);
}

void AN::TestServer::handle() {
	cout << buffer << endl;
	cout << "--------------------------------------------" << endl;
}

void AN::TestServer::respoder() {
	char hello[18] = "Hello from server";
	write(new_socket, hello, strlen(hello));
	// send(new_socket, hello, strlen(hello), 0);
	cout << "Hello message sent" << endl;
	close(new_socket);
}

void AN::TestServer::launch() {
	while (true) {
		cout << "Waiting for connection..." << endl;
		accepter();
		handle();
		respoder();
		cout << "Done" << endl;
	}
}