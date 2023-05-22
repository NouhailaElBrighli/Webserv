#include "Response.hpp"

Response::Response()
{

}

Response::~Response()
{

}


void	Response::SetFile(const std::string &RequestURI)
{
	std::stringstream ss(RequestURI);
	std::string filename;
	while (!getline(ss, filename, '/'))
	{
		std::cout << filename << std::endl;
	}
	std::cout << "last one" << filename << std::endl;
}

