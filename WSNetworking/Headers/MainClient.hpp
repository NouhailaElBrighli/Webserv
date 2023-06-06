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
	std::string			header;
	int					location;

  private:
	// Copy constructor and assignation operator
	MainClient(const MainClient &);
	MainClient &operator=(const MainClient &);

	string head, body;

  public:
	// Getters
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);

	const bool &get_send_receive_status() const;

	// Constructors and destructor
	MainClient();
	MainClient(int client_socket, ConfigServerParser *config_server_parser, string task);
	~MainClient();

	// Methods
	int		GetClientSocket();
	void	start_handle();
	void	replace_location();
	int		match_location();
	void	set_header_for_errors_and_redirection();
	void	set_location(int location);
	int		get_location();
	ConfigServerParser *get_config_server();
	void start(string task);

  private:
	// Methods
	void start_handle(string task);

	void Header_reading(int client_socket);
	void Body_reading(int client_socket);

	void handle_read(int client_socket);
	void handle_write(int client_socket);

	int	 get_matched_location_for_request_uri();
	void is_method_allowed_in_location();
};

#endif	// MAINCLIENT_HPP