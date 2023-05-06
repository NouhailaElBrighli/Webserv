#ifndef CONFIGFILEPARSER_HPP
#define CONFIGFILEPARSER_HPP

#include "WSNetworking.hpp"

class ConfigFileParser {

  private:
	// Attributes
	std::ifstream				 config_file;
	string						 config_file_path;
	string						 config_file_content;
	bool						 config_file_content_status;
	vector<string>				 config_file_server;
	vector<ConfigServerParser *> config_server_parser;

  public:
	// Getters
	vector<ConfigServerParser *> get_config_server_parser() const;

	// Constructors and copy constructor and copy assignment operator and destructor
	ConfigFileParser(string config_file_path);
	~ConfigFileParser();

  private:
	// Methods
	void open_config_file();
	void read_config_file();
	int	 get_start_end_server(size_t pos, string delimiter);
	void split_config_file();
	void parse_config_file();
};

#endif // CONFIGFILEPARSER_HPP