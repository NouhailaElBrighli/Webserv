#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "WSNetworking.hpp"

class Response {
  private:
	std::string ContentType;
	std::string	ContentLength;
	std::string header;
	std::string filename;

  public:
	Response();
	~Response();
	// SetContentType();
	// SetContentLength();
	void SetVars(const std::string &RequestURI, int client_socket);
};

#endif