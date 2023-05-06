#include "WSNetworking.hpp"

int main(int ac, char **av) {

	if (ac == 2) {
		cout << str_cyan("Run Config File Parser ...") << endl;

		ConfigFileParser config_file_parser(av[1]);

		try {
			config_file_parser.run();
			// config_file_parser.print_config_file();

			cout << str_green("Config File Parsed Successfully") << endl;
		} catch (const std::exception &e) {
			cerr << e.what() << endl;
			return EXIT_FAILURE;
		}

		try {
			cout << str_cyan("Run Main Server ...") << endl;

			vector<int> port;
			port.push_back(8080);
			port.push_back(18000);
			port.push_back(8888);

			MainServer main_server(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY, 10);

			cout << str_green("Main Server Terminate Successfully") << endl;
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