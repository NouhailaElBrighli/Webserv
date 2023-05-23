#ifndef MAINCLIENT_HPP
#define MAINCLIENT_HPP

#include "WSNetworking.hpp"

class MainClient {

  private:
	ConfigFileParser   *config_file_parser;
	ConfigServerParser *config_server_parser;
	RequestParser	   *request_parser;
	int					status;
	bool				send_receive_status;  //! STATUS OF SENDING AND RECIEVING DATA
	string				msg_status;
	int					client_socket, port;
	char				buffer[MAXLINE + 1];
	bool				server_parser_set;

	MainClient(const MainClient &);
	MainClient &operator=(const MainClient &);

  public:
	// Getters
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);

	const bool &get_send_receive_status() const;

	// Constructors and destructor
	MainClient();
	MainClient(int client_socket, ConfigServerParser *config_server_parser, int port,
			   bool server_parser_set);
	MainClient(int client_socket, ConfigFileParser *config_file_parser, int port,
			   bool server_parser_set);
	~MainClient();

	// Methods
	void start_handle();

  private:
	// Methods
	int	 get_right_server(string name_server);
	void handle(int client_socket);
	void responder(int client_socket);
	void get_matched_location_for_request_uri();
	void is_method_allowed_in_location();
};

#endif	// MAINCLIENT_HPP