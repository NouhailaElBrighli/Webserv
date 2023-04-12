#include "WSNetworking.hpp"

int main() {
	// cout << C_CYAN << "Test Sockets ..." << C_RES << endl;

	// try {
	// 	cout << "Binding socket ..." << endl;
	// 	WSN::BindingSocket binding_socket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
	// 	cout << C_GREEN << "Binding socket succeed" << C_RES << endl;
	// } catch (const std::exception &e) {
	// 	cerr << e.what() << endl;
	// }

	// try {
	// 	cout << "Listening socket ..." << endl;
	// 	WSN::ListeningSocket listening_socket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10);
	// 	cout << C_GREEN << "Listening socket succeed" << C_RES << endl;
	// } catch (const std::exception &e) {
	// 	cerr << e.what() << endl;
	// }

	// try {
	// 	cout << "Connecting socket ..." << endl;
	// 	WSN::ConnectingSocket connecting_socket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
	// 	cout << C_GREEN << "Connecting socket succeed" << C_RES << endl;
	// } catch (const std::exception &e) {
	// 	cerr << e.what() << endl;
	// }

	try {
		cout << C_CYAN << "Test Main Server ..." << C_RES << endl;
		// Create a listening socket for each port
		vector<int> port;

		port.push_back(8080);
		port.push_back(18000);
		port.push_back(66);

		WSN::MainServer test_server(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY, 10);
		cout << C_GREEN << "Test main server succeed" << C_RES << endl;
	} catch (const std::exception &e) {
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}