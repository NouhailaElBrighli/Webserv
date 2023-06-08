#ifndef MAINCLIENT_HPP
#define MAINCLIENT_HPP

#include "WSNetworking.hpp"
#include "Cgi.hpp"

class MainClient {

  private:
	ConfigServerParser *config_server_parser;
	RequestParser	   *request_parser;
	int					status;
	bool				send_receive_status;  //! STATUS OF SENDING AND RECEIVING DATA
	string				msg_status;
	int					client_socket;
	char				buffer[MAXLINE + 1];
	std::string			header;
	int					location;
	std::string			redirection;
	std::string			new_url;

	MainClient(const MainClient &);
	MainClient &operator=(const MainClient &);

  public:
	// Getters
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);

	const bool &get_send_receive_status() const;

	// Constructors and destructor
	MainClient();
	MainClient(int client_socket, ConfigServerParser *config_server_parser);
	~MainClient();
	std::string	 Header_reading(int client_socket);
	std::string &Body_reading(int client_socket, std::string &body);
	// Methods
	int		GetClientSocket();
	void	start_handle();
	void	replace_location();
	int		match_location();
	void	set_header_for_errors_and_redirection();
	void	set_location(int location);
	int		get_location();
	ConfigServerParser *get_config_server();
	void set_redirection(std::string &redirection);
	std::string get_new_url();

  private:
	// Methods
	void handle(int client_socket);
	void responder(int client_socket);
	int	 get_matched_location_for_request_uri();
	void is_method_allowed_in_location();
};

#endif	// MAINCLIENT_HPP