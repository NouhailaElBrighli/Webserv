#ifndef CONFIGSERVERPARSER_HPP
#define CONFIGSERVERPARSER_HPP

#include "WSNetworking.hpp"

class ConfigServerParser {

  private:
	// Attributes
	bool						   run_status;
	string						   config_server;
	int							   port;
	string						   port_str;
	bool						   port_status;
	vector<int>					   host_v;
	string						   host_s;
	bool						   host_status;
	string						   server_name;
	bool						   server_name_status;
	size_t						   client_max_body_size;
	bool						   client_max_body_size_status;
	map<int, string>			   error_page;
	bool						   error_page_status;
	vector<ConfigLocationParser *> config_location_parser;
	int							   config_location_parser_status;

  public:
	// Getters
	const int							 &get_port() const;
	const string						 &get_port_str() const;
	const int							 &get_host(int i) const;
	const string						 &get_host() const;
	const string						 &get_server_name() const;
	const size_t						 &get_client_max_body_size() const;
	const map<int, string>				 &get_error_page() const;
	const bool							 &get_error_page_status() const;
	const vector<ConfigLocationParser *> &get_config_location_parser() const;

	// Constructors and copy constructor and copy assignment operator and
	// destructor
	ConfigServerParser(string config_server);
	~ConfigServerParser();

	// Tools
	static int	  stringToInt(string str);
	static size_t stringToSize_t(string str);

	// Methods
	void parse_config_server();

  private:
	// Tools
	int			checkType(string str);
	vector<int> split_ip_address(const string &str);
	vector<int> stringToHost(string host);
	bool		find_compare(string &line, const string &str);

	// Setters
	void set_port(string port, size_t pos);
	void set_host(string host, size_t pos);
	void set_server_name(string server_name, size_t pos);
	void set_client_max_body_size(string client_max_body_size, size_t pos);
	void set_error_page(string error_page, size_t pos);
	void set_config_location_parser(string config_location);

	// Methods
	int	 get_start_end_location(string location, size_t pos);
	int	 split_config_location(string &location);
	void check_status();
};

std::ostream &operator<<(std::ostream &o, ConfigServerParser const &rhs);

#endif	// CONFIGSERVERPARSER_HPP