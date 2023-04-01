#include "ANNetworking.hpp"

int main() {
	// cout << C_CYAN << "Starting ..." << C_RES << endl;

	// cout << "Binding socket ..." << endl;
	// AN::BindingSocket binding_socket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
	// cout << C_GREEN << "Binding socket done" << C_RES << endl;

	// cout << "Connecting socket ..." << endl;
	// AN::ConnectingSocket connecting_socket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
	// cout << C_GREEN << "Conntcting socket done" << C_RES << endl;

	// cout << "Listening socket ..." << endl;
	// AN::ListeningSocket listening_socket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10);
	// cout << C_GREEN << "Listening socket done" << C_RES << endl;

	cout << C_CYAN << "Test Server" << C_RES << endl;

	cout << "Server ..." << endl;
	AN::TestServer server(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10);
	cout << C_GREEN << "Server done" << C_RES << endl;

	return 0;
}