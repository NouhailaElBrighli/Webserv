#include "WSNetworking.hpp"

int main(int ac, char **av) {

	if (ac == 2) {
		cout << str_cyan("Run Config File Parser ...") << endl;

		ConfigFileParser *config_file_parser = new ConfigFileParser(av[1]);

		try {
			config_file_parser->parse();
			config_file_parser->print_parsed_config_file();

			cout << str_green("Config File Parsed Successfully") << endl;
		} catch (const std::exception &e) {
			cerr << e.what() << endl;
			return EXIT_FAILURE;
		}

		cout << str_cyan("Run Main Server ...") << endl;

		MainServer main_server(AF_INET, SOCK_STREAM, 0, config_file_parser, INADDR_ANY, 10);

		try {
			main_server.launch();

			cout << str_green("Main Server Terminate Successfully") << endl;
		} catch (const std::exception &e) {
			cerr << e.what() << endl;
			return EXIT_FAILURE;
		}

		delete config_file_parser;

	} else {
		cerr << "Usage: " << av[0] << " <config_file_path>" << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}