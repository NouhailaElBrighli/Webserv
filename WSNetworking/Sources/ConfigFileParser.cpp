/*
server {
	listen 8001;                                # listening port, mandatory parameter
	host 127.0.0.1;                             # host or 127.0.0.1 by default
	server_name test;                           # specify server_name, need to be added into /etc/hosts to work
	error_page 404 /error/404.html;             # default error page
	client_max_body_size 1024;                  # max request body size in bytes
	root docs/fusion_web/;                      # root folder of site directory, full or relative path, mandatory parameter
	index index.html;                           # default page when requesting a directory, index.html by default

	location /tours {
		root docs/fusion_web;                   # root folder of the location, if not specified, taken from the server.
												# EX: - URI /tours           --> docs/fusion_web/tours
												#     - URI /tours/page.html --> docs/fusion_web/tours/page.html
		autoindex on;                           # turn on/off directory listing
		allow_methods POST GET;                 # allowed methods in location, GET only by default
		index index.html;                       # default page when requesting a directory, copies root index by default
		return abc/index1.html;                 # redirection
		alias  docs/fusion_web;                 # replaces location part of URI.
												# EX: - URI /tours           --> docs/fusion_web
												#     - URI /tours/page.html --> docs/fusion_web/page.html
  }

	location cgi-bin {
		root ./;                                # cgi-bin location, mandatory parameter
		cgi_path /usr/bin/python3 /bin/bash;    # location of interpreters installed on the current system, mandatory parameter
		cgi_ext .py .sh;                        # extensions for executable files, mandatory parameter
  }
}
*/

/*
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class ConfigFileParser {
  public:
	ConfigFileParser(const std::string &filename) : filename_(filename) {}

	bool parse() {
		std::ifstream file(filename_);
		if (!file.is_open()) {
			std::cerr << "Failed to open file: " << filename_ << std::endl;
			return false;
		}

		std::string						   line;
		std::string						   current_server;
		std::string						   current_location;
		std::map<std::string, std::string> server_properties;
		std::map<std::string, std::string> location_properties;

		while (std::getline(file, line)) {
			if (line.empty() || line[0] == '#') {
				continue; // skip empty lines and comments
			}

			size_t pos = line.find_first_not_of(' ');
			if (pos == std::string::npos) {
				continue; // skip lines with only whitespace
			}

			line = line.substr(pos);

			if (line.find("server") == 0) {
				if (!current_server.empty()) {
					servers_.push_back({current_server, server_properties});
					server_properties.clear();
				}
				current_server = line;
				current_location.clear();
			} else if (line.find("location") == 0) {
				if (!current_location.empty()) {
					locations_[current_server].push_back({current_location, location_properties});
					location_properties.clear();
				}
				current_location = line;
			} else {
				size_t equals_pos = line.find('=');
				if (equals_pos == std::string::npos) {
					std::cerr << "Invalid line: " << line << std::endl;
					return false;
				}

				std::string key	  = line.substr(0, equals_pos);
				std::string value = line.substr(equals_pos + 1);

				if (current_location.empty()) {
					server_properties[key] = value;
				} else {
					location_properties[key] = value;
				}
			}
		}

		if (!current_server.empty()) {
			servers_.push_back({current_server, server_properties});
		}

		if (!current_location.empty()) {
			locations_[current_server].push_back({current_location, location_properties});
		}

		return true;
	}

	void print_config() const {
		for (const auto &server : servers_) {
			std::cout << server.first << std::endl;
			for (const auto &prop : server.second) {
				std::cout << "  " << prop.first << " = " << prop.second << std::endl;
			}

			const auto &server_locations = locations_.find(server.first);
			if (server_locations != locations_.end()) {
				for (const auto &location : server_locations->second) {
					std::cout << "  " << location.first << std::endl;
					for (const auto &prop : location.second) {
						std::cout << "    " << prop.first << " = " << prop.second << std::endl;
					}
				}
			}
		}
	}

  private:
	std::string																					   filename_;
	std::vector<std::pair<std::string, std::map<std::string, std::string>>>						   servers_;
	std::map<std::string, std::vector<std::pair<std::string, std::map<std::string, std::string>>>> locations_;
};

int main() {
	ConfigFileParser parser("config.txt");
	if (parser.parse()) {
	}
}
*/

#include "ConfigFileParser.hpp"

// Getters
vector<ConfigServerParser *> ConfigFileParser::get_config_server_parser() const {
	return this->config_server_parser;
}

// Constructors and copy constructor and copy assignment operator and destructor
ConfigFileParser::ConfigFileParser(string config_file_path) : config_file(config_file_path), config_file_path(config_file_path) {
	try {
		this->open_config_file();
		this->read_config_file();
		this->split_config_file();
		this->parse_config_file();
	} catch (const std::exception &e) {
		cerr << e.what() << endl;
	}
}

ConfigFileParser::~ConfigFileParser() {
	// for (vector<ConfigServerParser *>::iterator it = this->config_server_parser.begin(); it != this->config_server_parser.end(); it++) {
	// 	delete *it;
	// }
}

// Methods
void ConfigFileParser::open_config_file() {
	if (!config_file.is_open()) {
		throw std::runtime_error("Failed to open file: " + this->config_file_path);
	}
	cout << "File opened" << endl;
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
	if ((pos = this->config_file_content.find(delimiter)) != std::string::npos) {
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

// split by "srever {" but the first one is also "server {"
void ConfigFileParser::split_config_file() {
	string server;
	size_t server_size = 0;
	size_t pos		   = 0;
	string delimiter   = "server {";

	while ((pos = this->config_file_content.find(delimiter)) != std::string::npos) {
		server_size = this->get_start_end_server(pos) - delimiter.length();
		server		= this->config_file_content.substr(delimiter.length() + 1, server_size - 2);
		this->config_file_content.erase(0, server_size);
		this->config_file_server.push_back(server);
	}
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
}
