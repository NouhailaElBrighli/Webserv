#include "WSNetworking.hpp"

int main() {
	cout << C_CYAN << "Test Sockets ..." << C_RES << endl;

	try {
		cout << "Binding socket ..." << endl;
		WSN::BindingSocket binding_socket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
		cout << C_GREEN << "Binding socket succeed" << C_RES << endl;
	} catch (const std::exception &e) {
		cerr << e.what() << endl;
	}

	try {
		cout << "Listening socket ..." << endl;
		WSN::ListeningSocket listening_socket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10);
		cout << C_GREEN << "Listening socket succeed" << C_RES << endl;
	} catch (const std::exception &e) {
		cerr << e.what() << endl;
	}

	try {
		cout << "Connecting socket ..." << endl;
		WSN::ConnectingSocket connecting_socket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
		cout << C_GREEN << "Connecting socket succeed" << C_RES << endl;
	} catch (const std::exception &e) {
		cerr << e.what() << endl;
	}

	cout << C_CYAN << "Test Server ..." << C_RES << endl;

	cout << "Server ..." << endl;
	WSN::TestServer server(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10);
	cout << C_GREEN << "Server succeed" << C_RES << endl;

	return 0;
}