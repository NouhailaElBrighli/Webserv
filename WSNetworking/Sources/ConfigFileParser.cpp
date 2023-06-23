#include "ConfigFileParser.hpp"

// Getters
vector<ConfigServerParser *> ConfigFileParser::get_config_server_parser() const { return this->config_server_parser; }

ConfigServerParser *ConfigFileParser::get_config_server_parser(int index) const {
	return this->config_server_parser[index];
}

// Constructors and copy constructor and copy assignment operator and destructor
ConfigFileParser::ConfigFileParser(string config_file_path)
	: parse_status(false), config_file(config_file_path.c_str()), config_file_path(config_file_path),
	  config_file_content_status(false) {}

ConfigFileParser::~ConfigFileParser() {
	for (vector<ConfigServerParser *>::iterator it = this->config_server_parser.begin();
		 it != this->config_server_parser.end(); it++) {
		delete *it;
	}
}

// Methods
void ConfigFileParser::parse() {
	if (this->parse_status) {
		return;
	}
	this->open_config_file();
	this->read_config_file();
	this->split_config_file();
	this->parse_config_file();
	this->parse_status = true;
}

void ConfigFileParser::open_config_file() {
	if (!config_file.is_open()) {
		throw std::runtime_error(STR_RED("Failed to open file: " + this->config_file_path));
	}
}

void ConfigFileParser::read_config_file() {
	string line;
	while (std::getline(this->config_file, line)) {
		if (line.empty()) {
			continue;  // skip empty lines
		}

		size_t pos = line.find_first_not_of(' ');
		if (pos == string::npos) {
			continue;  // skip lines with only whitespace
		}

		if (line[0] == '#' || line[pos] == '#') {
			continue;  // skip comments
		}

		line = line.substr(pos);
		this->config_file_content += (line) + "\n";
	}
	// check if empty file
	if (this->config_file_content.empty()) {
		throw std::runtime_error(STR_RED("Empty file: " + this->config_file_path));
	}
	this->config_file_content += "\0";
}

// by counting the brackets to get the number of servers
size_t ConfigFileParser::get_start_end_server(size_t pos, string delimiter) {
	size_t count   = 0;
	int	   bracket = 0;

	if ((count = this->config_file_content.find(delimiter)) != string::npos) {
		bracket++;
		count += pos + delimiter.length();
	}
	while (this->config_file_content[count]) {
		if (this->config_file_content[count] == '{') {
			bracket++;
		} else if (this->config_file_content[count] == '}') {
			bracket--;
		}
		count++;
		if (bracket == 0) {
			break;
		}
	}

	if (this->config_file_content[count] == '\0') {
		throw std::runtime_error(STR_RED("Missing closing bracket in file: " + this->config_file_path));
	}

	return count;
}

void ConfigFileParser::split_config_file() {
	string server;
	size_t server_size = 0;
	size_t pos		   = 0;
	string delimiter   = "server {";

	while ((pos = this->config_file_content.find(delimiter)) != string::npos) {
		server_size = this->get_start_end_server(pos, delimiter) - delimiter.length();
		server		= this->config_file_content.substr(delimiter.length() + 1, server_size - 2);
		this->config_file_content.erase(0, server_size + delimiter.length() + 1);
		this->config_file_server.push_back(server);
		this->config_file_content_status = true;
	}
	if (!this->config_file_content_status) {
		throw std::runtime_error(STR_RED("No server found in file: " + this->config_file_path));
	}
}

void ConfigFileParser::parse_config_file() {
	for (vector<string>::iterator it = this->config_file_server.begin(); it != this->config_file_server.end(); it++) {
		ConfigServerParser *config_server_parser = new ConfigServerParser(*it);
		try {
			config_server_parser->parse_config_server();
		} catch (const std::exception &e) {
			delete config_server_parser;
			throw std::runtime_error(e.what());
		}

		// check duplicate server_name in previews servers
		for (vector<ConfigServerParser *>::iterator it2 = this->config_server_parser.begin();
			 it2 != this->config_server_parser.end(); it2++) {
			if (config_server_parser->get_server_name() == (*it2)->get_server_name()) {
				string port_str = config_server_parser->get_port_str();
				delete config_server_parser;
				throw std::runtime_error(STR_RED("Duplicate server_name: '" + (*it2)->get_server_name()
												 + "' in server with port: '" + port_str
												 + "' in file: " + this->config_file_path));
			}
		}
		this->config_server_parser.push_back(config_server_parser);
	}
}

void ConfigFileParser::print_parsed_config_file() {
	for (vector<ConfigServerParser *>::iterator it = this->config_server_parser.begin();
		 it != this->config_server_parser.end(); it++) {
		cout << **it;
	}
}
