#include "WSNetworking.hpp"

int main(int ac, char **av) {

	if (ac == 2) {
		cout << STR_CYAN("Run Config File Parser ...") << endl;

		ConfigFileParser *config_file_parser = new ConfigFileParser(av[1]);

		try {
			config_file_parser->parse();
			// config_file_parser->print_parsed_config_file();

			cout << STR_GREEN("Config File Parsed Successfully") << endl;
		} catch (const std::exception &e) {
			delete config_file_parser;
			cerr << e.what() << endl;
			return EXIT_FAILURE;
		}

		cout << STR_CYAN("Run Main Server ...") << endl;

		MainServer main_server(config_file_parser, 10);

		try {

			main_server.launch();
			cout << STR_GREEN("Main Server Terminate Successfully") << endl;
		} catch (const std::exception &e) {
			delete config_file_parser;
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