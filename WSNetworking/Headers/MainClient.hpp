#ifndef MAINCLIENT_HPP
#define MAINCLIENT_HPP

#include "WSNetworking.hpp"

class MainClient {

  private:
	ConfigServerParser *config_server_parser;
	RequestParser	   *request_parser;
	bool				send_receive_status;
	string				msg_status;
	int					client_socket;
	char				buffer[MAXLINE + 1];
	string				header;
	int					location;
	int					status, phase;

  private:
	// Copy constructor and assignation operator
	MainClient(const MainClient &);
	MainClient &operator=(const MainClient &);

	string head, body, body_file;
	bool   head_status, body_status;

  public:
	// Getters
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);
	const bool				  &get_send_receive_status() const;
	const int				  &get_phase() const;

	int					get_client_socket();
	int					get_location();
	ConfigServerParser *get_config_server();

	// Setters
	void set_location(int location);

	// Constructors and destructor
	MainClient();
	MainClient(int client_socket, ConfigServerParser *config_server_parser, string task);
	~MainClient();

	// Methods
	void start_handle();
	void replace_location();
	void start(string task);

  private:
	// Methods
	void start_handle(string task);

	void   header_reading();
	string generate_random_file_name();
	void   body_reading();

	void handle_read();
	void handle_write();

	int check_and_change_request_uri();
	// int	 get_matched_location_for_request_uri();
	void is_method_allowed_in_location();

	void set_header_for_errors_and_redirection();
};

#endif	// MAINCLIENT_HPP