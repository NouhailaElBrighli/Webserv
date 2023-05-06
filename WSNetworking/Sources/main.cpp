#include "WSNetworking.hpp"

void binding_socket_test() {
	cout << str_cyan("Test Binding Socket ...") << endl;

	try {
		cout << "Binding Socket ..." << endl;
		BindingSocket binding_socket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);
		cout << str_green("Binding Socket Succeed") << endl;
	} catch (const std::exception &e) {
		cerr << e.what() << endl;
	}
}

void listening_socket_test() {
	cout << str_cyan("Test Listening Socket ...") << endl;

	try {
		cout << "Listening socket ..." << endl;
		ListeningSocket listening_socket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10);
		cout << str_green("Listening Socket Succeed") << endl;
	} catch (const std::exception &e) {
		cerr << e.what() << endl;
	}
}

void config_file_parser_test(char *config_file_path) {
	cout << str_cyan("Test Config File Parser ...") << endl;
	ConfigFileParser config_file_parser(config_file_path);
	cout << str_green("Test Config File Parser Succeed") << endl;
}

void main_server_run() {
	cout << str_cyan("Test Main Server ...") << endl;
	// Create a listening socket for each port
	vector<int> port;

	port.push_back(8080);
	port.push_back(18000);
	port.push_back(8888);

	MainServer main_server(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY, 10);
	cout << str_green("Test Main Server Succeed") << endl;
}

int main(int ac, char **av) {

	if (ac == 2) {
		// binding_socket_test();

		// listening_socket_test();

		try {
			config_file_parser_test(av[1]);
			// main_server_run();
		} catch (const std::exception &e) {
			cerr << e.what() << endl;
			return EXIT_FAILURE;
		}
	} else {
		cerr << "Usage: " << av[0] << " <config_file_path>" << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}