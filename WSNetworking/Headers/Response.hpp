#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "WSNetworking.hpp"

class Response
{
	private :
		std::string	ContentType;
		int			ContentLength;
		std::string	header;
	public :
		Response();
		~Response();
		// SetContentType();
		// SetContentLength();
		void	SetFile(const std::string &RequestURI);
};

#endif