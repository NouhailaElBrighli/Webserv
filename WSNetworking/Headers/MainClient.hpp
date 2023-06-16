#ifndef MAINCLIENT_HPP
#define MAINCLIENT_HPP

#include "WSNetworking.hpp"

class MainClient {

  private:
	ConfigServerParser				  *config_server_parser;
	RequestParser					  *request_parser;
	bool							   send_receive_status;
	string							   msg_status;
	int								   client_socket, status, phase;
	int								   php_status;
	bool							   write_header, write_body, write_status; 
	bool							   file_open;
	HeaderBodyReader				  *header_body_reader;


  private:
	std::map<std::string, std::string> content_type;
	std::map<std::string, std::string> extention;
	map<string, string>				   type_mime;
	std::streampos					   position;
	string							   header;
	int								   location;
	std::string						   redirection;
	std::string						   new_url;
	std::string						   serve_file;
	std::string						   body_file;
	int								   start_php;


  private:
	// Copy constructor and assignation operator
	MainClient();
	MainClient(const MainClient &);
	MainClient &operator=(const MainClient &);

  public:
	// Getters
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);
	const bool				  &get_send_receive_status() const;
	const int				  &get_phase() const;
	const string			  &get_body_file_name() const;
	const int				  &get_client_socket() const;
	const int				  &get_location() const;
	ConfigServerParser		  *get_config_server() const;
	std::string				   get_content_type(std::string extention);
	const map<string, string> &get_mime_type() const;
	const string			  &get_mime_type(const string &type) const;

	// Setters
	void set_send_receive_status(bool send_receive_status);
	void set_location(int location);
	void set_header(std::string header);
	void reset_body_file_name(std::string new_name);

	// Constructors and destructor
	MainClient(int client_socket, ConfigServerParser *config_server_parser);
	~MainClient();

	// Methods
	int			GetClientSocket();
	void		set_header_for_errors_and_redirection(const char *what);
	void		set_redirection(std::string &redirection);
	std::string get_new_url();
	void		set_extention_map();
	void		set_start_php(int start);
	std::string	get_extention(std::string content);

	std::string get_serve_file();
	std::string write_into_file(DIR *directory, std::string root);
	int			convert_to_int(const std::string &str);
	void		set_serve_file(std::string file_to_serve);
	void		send_to_socket();
	void		set_content_type_map();

	void start(string task);

  private:
	// Methods
	void start_handle(string task);
	void handle_read();
	void handle_write();

	int		match_location();
	void	is_method_allowed_in_location();
	void	check_files_error();
	int		check_for_root_directory();
};

#endif	// MAINCLIENT_HPP