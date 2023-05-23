#include "Response.hpp"

Response::Response() {
}

Response::~Response() {
}

void Response::SetVars(const std::string &RequestURI, int client_socket) {
	std::stringstream ss(RequestURI);
	std::string		  filename;
	std::string		  data;
	// std::string str;
	while (getline(ss, filename, '/'))

		this->filename = filename;
	int start = filename.find('.');
	// check if there is no extention
	std::string extention = filename.substr(start, filename.size() - 1);
	if (extention == "html")
		this->ContentType = "text/html";
	else if (extention == "css")
		this->ContentType = "text/css";
	else if (extention == "jpg" || extention == "jpeg")
		this->ContentType = "image/jpeg";
	else if (extention == "png")
		this->ContentType = "image/png";
	else
		this->ContentType = "text/plain";
	std::ifstream RequestedFile(RequestURI);
	if (!RequestedFile.is_open())
		throw std::runtime_error("couldn't open the file");
	getline(RequestedFile, data, '\0');
	int				  length = data.size();
	std::stringstream num;
	num << length;
	this->ContentLength = num.str();
	std::cout << "data:\n"
			  << data << std::endl;
	std::cout << "extention:" << extention << std::endl;
	std::cout << "ContentType:" << ContentType << std::endl;
	std::cout << "ContentLength:" << ContentLength << std::endl;
	this->header = "HTTP/1.1 200 ok\r\nContent-Type: ";
	this->header += this->ContentType;
	this->header += "\r\nContent-Length: ";
	this->header += ContentLength;
	this->header += "\r\n\r\n";
	this->header += data;
	std::cout << header << std::endl;
	send(client_socket, header.c_str(), header.size(), 0);
}
