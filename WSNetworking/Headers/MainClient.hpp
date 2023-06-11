#ifndef MAINCLIENT_HPP
#define MAINCLIENT_HPP

#include "WSNetworking.hpp"
#include "Cgi.hpp"

class MainClient {

  private:
  	std::map<std::string, std::string>content_type;
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
	std::string			serve_file;
	std::string			body_file;

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
	void	set_header_for_errors_and_redirection(const char *what);
	void	set_location(int location);
	int		get_location();
	ConfigServerParser *get_config_server();
	void set_redirection(std::string &redirection);
	std::string get_new_url();
	std::string	get_serve_file();
	std::string	write_into_file(DIR *directory, std::string root);
	int	convert_to_int(std::string	&str);
	void	set_serve_file(std::string file_to_serve);
	void	set_header(std::string header);
	void	send_to_socket();
	void	set_content_type_map();

  private:
	// Methods
	void handle(int client_socket);
	void responder(int client_socket);
	int	 get_matched_location_for_request_uri();
	void is_method_allowed_in_location();
	void	check_if_uri_exist();
	void	check_files_error();
};

#endif	// MAINCLIENT_HPP