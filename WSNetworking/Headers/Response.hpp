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
	void SetError(const std::string msg_status);
	void	check_request_uri();
	void	Get(MainClient *client);
    void	set_resource_type();
	void	check_inside_root(std::string &root, std::string path);
	std::string	handle_directory();
	void	serve_file(std::string	index_file);
	std::string	check_auto_index();
	std::string	handle_file();
};

std::ostream &operator<<(std::ostream &out, const Response &obj);//!here

#endif