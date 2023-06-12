#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "WSNetworking.hpp"

class Response {
  private:
	std::string ContentType;
	std::string	ContentLength;
	std::string filename;
	std::string body;
	std::string header;
	MainClient	*Client;
	std::string	type;
	std::string outfile_cgi;
	int	cgi_status;
  public:
	Response();
	~Response();
	Response(MainClient *Client);
	void SetVars(std::string file_to_serve);
	void SetContentType();
	void SetContentLength(std::string RequestURI);
	std::string GetContentType() const;
	std::string GetContentLength() const;
	std::string GetHeader() const;
	std::string SetError(const std::string msg_status, std::string body_file);
	void	check_request_uri();
	std::string	Get(MainClient *client);
    void	set_resource_type();
	void	check_inside_root(std::string &root, std::string path);
	std::string	handle_directory();
	void	serve_file(std::string	index_file);
	std::string	check_auto_index();
	std::string	handle_file();
	std::string	set_error_body(std::string msg_status,std::string body_file);
	std::string post(MainClient *Client);
};

std::ostream &operator<<(std::ostream &out, const Response &obj);//!here

#endif