#include "ConfigFileParser.hpp"

// Getters
vector<ConfigServerParser *> ConfigFileParser::get_config_server_parser() const {
	return this->config_server_parser;
}

// Constructors and copy constructor and copy assignment operator and destructor
ConfigFileParser::ConfigFileParser(string config_file_path) : config_file(config_file_path), config_file_path(config_file_path) {

	this->open_config_file();
	this->read_config_file();
	this->split_config_file();
	this->parse_config_file();
}

ConfigFileParser::~ConfigFileParser() {
	for (vector<ConfigServerParser *>::iterator it = this->config_server_parser.begin(); it != this->config_server_parser.end(); it++) {
		delete *it;
	}
}

// Methods
void ConfigFileParser::open_config_file() {
	if (!config_file.is_open()) {
		throw std::runtime_error("Failed to open file: " + this->config_file_path);
	}
}

void ConfigFileParser::read_config_file() {
	std::string line;
	while (std::getline(this->config_file, line)) {
		if (line.empty() || line[0] == '#') {
			continue; // skip empty lines and comments
		}

		size_t pos = line.find_first_not_of(' ');
		if (pos == std::string::npos) {
			continue; // skip lines with only whitespace
		}

		line = line.substr(pos);
		this->config_file_content += (line) + "\n";
	}
}

// by counting the brackets to get the number of servers
int ConfigFileParser::get_start_end_server(size_t pos) {
	int	   count	 = 0;
	size_t bracket	 = 0;
	string delimiter = "server {";
	if ((count = this->config_file_content.find(delimiter)) != std::string::npos) {
		bracket++;
		count += pos + delimiter.length();
	}
	while (bracket > 0) {
		if (this->config_file_content[count] == '{') {
			bracket++;
		} else if (this->config_file_content[count] == '}') {
			bracket--;
		}
		if (bracket == -1) {
			break;
		}
		count++;
	}

	return count;
}

void ConfigFileParser::split_config_file() {
	string server;
	size_t server_size = 0;
	size_t pos		   = 0;
	string delimiter   = "server {";

	while ((pos = this->config_file_content.find(delimiter)) != std::string::npos) {
		server_size = this->get_start_end_server(pos) - delimiter.length();
		server		= this->config_file_content.substr(delimiter.length() + 1, server_size - 2);
		this->config_file_content.erase(0, server_size + delimiter.length() + 1);
		this->config_file_server.push_back(server);
	}
	// print splitted servers
	int i = 1;
	for (vector<string>::iterator it = this->config_file_server.begin(); it != this->config_file_server.end(); it++) {
		print_line("Server " + std::to_string(i++));
		cout << *it << endl;
	}
	print_line("Done");
}

void ConfigFileParser::parse_config_file() {
	for (vector<string>::iterator it = this->config_file_server.begin(); it != this->config_file_server.end(); it++) {
		ConfigServerParser *config_server_parser = new ConfigServerParser(*it);
		this->config_server_parser.push_back(config_server_parser);
	}
	// print parsed servers
	for (vector<ConfigServerParser *>::iterator it = this->config_server_parser.begin(); it != this->config_server_parser.end(); it++) {
		cout << **it;
	}
}
