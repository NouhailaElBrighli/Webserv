#ifndef MAINCLIENT_HPP
#define MAINCLIENT_HPP

#include "WSNetworking.hpp"

class MainClient {

  private:
	ConfigFileParser   *config_file_parser;
	ConfigServerParser *config_server_parser;
	RequestParser	   *request_parser;
	int					status;
	string				msg_status;
	int					client_socket, port;
	char				buffer[MAXLINE + 1];

	MainClient(const MainClient &);
	MainClient &operator=(const MainClient &);

  public:
	// Getters
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);

	const int	 &get_status() const;
	const string &get_msg_status() const;

	// Constructors and destructor
	MainClient();
	MainClient(int client_socket, ConfigServerParser *config_server_parser,
			   int port);
	MainClient(int client_socket, ConfigFileParser *config_file_parser,
			   int port);
	~MainClient();

  private:
	// Methods
	int	 get_right_config_server_parser_from_name_sever(string name_server);
	void handle(int client_socket);
	void get_matched_location_for_request_uri();
	void is_method_allowded_in_location();
};

#endif	// MAINCLIENT_HPP