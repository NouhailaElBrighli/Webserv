#ifndef MAINCLIENT_HPP
#define MAINCLIENT_HPP

#include "WSNetworking.hpp"
#include "Cgi.hpp"

class MainClient {

  private:
	ConfigFileParser   *config_file_parser;
	ConfigServerParser *config_server_parser;
	RequestParser	   *request_parser;
	int					status;
	bool				send_recieve_status;  //! STATUS OF SENDING AND RECIEVING DATA
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

	const int	 &get_status() const;
	const string &get_msg_status() const;

	const bool &get_send_recieve_status() const;

	// Constructors and destructor
	MainClient();
	MainClient(int client_socket, ConfigServerParser *config_server_parser, int port,
			   bool server_parser_set);
	MainClient(int client_socket, ConfigFileParser *config_file_parser, int port, bool server_parser_set);
	~MainClient();

  private:
	// Methods
	void start_handle();
	int	 get_right_server(string name_server);
	void handle(int client_socket);
	void responder(int client_socket);
	void get_matched_location_for_request_uri();
	void is_method_allowded_in_location();
};

#endif	// MAINCLIENT_HPP