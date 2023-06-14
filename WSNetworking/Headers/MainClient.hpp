#ifndef MAINCLIENT_HPP
#define MAINCLIENT_HPP

#include "WSNetworking.hpp"

class MainClient {

  private:
  	std::map<std::string, std::string>content_type;
	ConfigServerParser *config_server_parser;
	RequestParser	   *request_parser;
	std::ifstream		*send_file;
	bool				send_receive_status;
	string				msg_status;
	int					client_socket;
	char				buffer[MAXLINE];
	string				header;
	int					location;
	std::string			redirection;
	std::string			new_url;
	std::string			serve_file;
	std::string			body_file;
	int					status, phase;
	int					start_php;
	int					php_status;
	bool				write_header;
	bool				write_body;
	bool				file_open;

  private:
	// Copy constructor and assignation operator
	MainClient(const MainClient &);
	MainClient &operator=(const MainClient &);

	string head, body, body_file_name;
	bool   head_status, body_status;

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
	std::string				get_content_type(std::string extention);

	// Setters
	void set_send_receive_status(bool send_receive_status);
	void set_location(int location);
	void set_header(std::string header);

	// Constructors and destructor
	MainClient();
	MainClient(int client_socket, ConfigServerParser *config_server_parser);
	~MainClient();

	// Methods
	int		GetClientSocket();
	void	set_header_for_errors_and_redirection(const char *what);
	void set_redirection(std::string &redirection);
	std::string get_new_url();
	std::string	get_serve_file();
	std::string	write_into_file(DIR *directory, std::string root);
	int	convert_to_int(std::string	&str);
	void	set_serve_file(std::string file_to_serve);
	void	send_to_socket();
	void	set_content_type_map();
	void	set_start_php(int start);


	void start_handle();
	void start(string task);

  private:
	// Methods
	void start_handle(string task);

	void header_reading();

	string generate_random_file_name();
	void   body_reading();

	int	 find_chunk_size0();
	int	 find_chunk_size1();
	void chunked_body_reading();

	void handle_read();
	void handle_write();

	void set_header_for_errors_and_redirection();
	int		match_location();
	void	is_method_allowed_in_location();
	void	check_files_error();
	int		check_for_root_directory();
};

#endif	// MAINCLIENT_HPP