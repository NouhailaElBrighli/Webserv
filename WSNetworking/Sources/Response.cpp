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
	if (extention == ".html")
		this->ContentType = "text/html";
	else if (extention == ".css")
		this->ContentType = "text/css";
	else if (extention == ".jpg" || extention == ".jpeg")
		this->ContentType = "image/jpeg";
	else if (extention == ".png")
		this->ContentType = "image/png";
	else
		this->ContentType = "text/plain";
	std::ifstream RequestedFile(RequestURI);
	if (!RequestedFile.is_open())
		throw std::runtime_error("couldn't open the file");
	getline(RequestedFile, this->body, '\0');
	int				  length = this->body.size();
	std::stringstream num;
	num << length;
	this->ContentLength = num.str();
	this->header = "HTTP/1.1 200 ok\r\nContent-Type: ";
	this->header += this->ContentType;
	this->header += "\r\nContent-Length: ";
	this->header += ContentLength;
	this->header += "\r\n\r\n";
	this->header += body;
}

std::string Response::GetContentType() const
{
	return(this->ContentType);
}
std::string Response::GetContentLength() const
{
	return(this->ContentLength);
}
std::string Response::GetHeader() const
{
	return(this->header);
}

void	Response::Get(std::string request_URI, int client_socket)
{
	print_long_line("Handle GET");
	this->SetVars(request_URI, client_socket);
	send(client_socket, header.c_str(), header.size(), 0);
	std::cout << *this << std::endl;
}

void 	Response::SetError(const std::string msg_status)
{
	std::stringstream ss(msg_status);
	std::stringstream num;
	std::string error;
	getline(ss, error, ' ');
	getline(ss, error, '\0');
	num << error.size();
	this->ContentLength = num.str();
	this->header = "HTTP/1.1 ";
	this->header += msg_status;
	this->header += "\r\nContent-Type: text/plain\r\n";
	this->header += "Content-Length: ";
	this->header += this->ContentLength;
	this->header += "\r\n\r\n";
	this->header += error;
}

std::ostream &operator<<(std::ostream &out,const Response &obj)
{
	out << "ContentType:" << obj.GetContentType() << std::endl;
	out << "ContentLength:" << obj.GetContentLength() << std::endl;
	out << "Header :" << std::endl;
	out << obj.GetHeader() << std::endl;
	return(out);
}