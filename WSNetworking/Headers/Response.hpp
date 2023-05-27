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
  public:
	Response();
	~Response();
	// SetContentType();
	// SetContentLength();
	void SetVars(const std::string &RequestURI, int client_socket);
	std::string GetContentType() const;
	std::string GetContentLength() const;
	std::string GetHeader() const;
	void SetError(const std::string msg_status);
	void	Get(std::string request_URI, int client_socket);
	
};

std::ostream &operator<<(std::ostream &out, const Response &obj);//!here

#endif