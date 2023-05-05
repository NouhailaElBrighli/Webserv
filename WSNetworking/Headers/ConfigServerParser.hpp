#ifndef CONFIGSERVERPARSER_HPP
#define CONFIGSERVERPARSER_HPP

#include "WSNetworking.hpp"

class ConfigServerParser {

  private:
	// Attributes
	int				 port;
	bool			 port_set;
	vector<int>		 host;
	bool			 host_set;
	string			 server_name;
	bool			 server_name_set;
	size_t			 client_max_body_size;
	bool			 client_max_body_size_set;
	map<int, string> error_page;
	// vector<ConfigLocationParser *> config_location_parser;

  public:
	// Getters
	int				 get_port() const;
	vector<int>		 get_host() const;
	string			 get_server_name() const;
	size_t			 get_client_max_body_size() const;
	map<int, string> get_error_page() const;
	// vector<ConfigLocationParser *> get_config_location_parser() const;

	// Constructors and copy constructor and copy assignment operator and destructor
	ConfigServerParser(string config_server);
	~ConfigServerParser();

  private:
	// Tools
	string		red_string(string txt);
	int			checkType(string str);
	int			stringToInt(string str);
	vector<int> split_ip_address(const string &str);
	vector<int> stringToHost(string host);

	// Setters
	void set_port(string port);
	void set_host(string host);
	void set_server_name(string server_name);
	void set_client_max_body_size(string client_max_body_size);
	void set_error_page(string error_page);
	// void set_config_location_parser(vector<ConfigLocationParser *> config_location_parser);

	// Methods
	void parse_config_server(string config_server);
};

#endif // CONFIGSERVERPARSER_HPP