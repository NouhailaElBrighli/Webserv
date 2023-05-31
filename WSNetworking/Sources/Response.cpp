#include "Response.hpp"

Response::Response() {}

Response::~Response() {}

std::string Response::GetContentType() const { return (this->ContentType); }

std::string Response::GetContentLength() const { return (this->ContentLength); }

std::string Response::GetHeader() const { return (this->header); }

void Response::Get(std::string request_URI, int client_socket) {
	print_long_line("Handle GET");
	this->SetVars(request_URI);
	send(client_socket, header.c_str(), header.size(), 0);
	// std::cout << *this << std::endl;
}

void Response::SetError(const std::string msg_status) {
	std::stringstream ss(msg_status);
	std::stringstream num;
	std::string		  error;
	getline(ss, error, ' ');
	getline(ss, error, '\0');
	num << error.size();
	this->ContentLength = num.str();
	this->header		= "HTTP/1.1 ";
	this->header += msg_status;
	this->header += "\r\nContent-Type: text/plain\r\n";
	this->header += "Content-Length: ";
	this->header += this->ContentLength;
	this->header += "\r\n\r\n";
	this->header += error;
}

std::ostream &operator<<(std::ostream &out, const Response &obj) {
	out << "ContentType:" << obj.GetContentType() << std::endl;
	out << "ContentLength:" << obj.GetContentLength() << std::endl;
	out << "Header :" << std::endl;
	out << obj.GetHeader() << std::endl;
	return (out);
}

void Response::SetContentType() {
	std::string extention;

	size_t start = this->filename.find('.');
	if (start != string::npos) {
		std::string extention = filename.substr(start, filename.size() - 1);
		if (extention == ".html")
			this->ContentType = "text/html";
		else if (extention == ".css")
			this->ContentType = "text/css";
		else if (extention == ".jpg" || extention == ".jpeg")
			this->ContentType = "image/jpeg";
		else if (extention == ".png")
			this->ContentType = "image/png";
		else if (extention == ".mp4")
			this->ContentType = "video/mp4";
		else
			this->ContentType = "text/plain";
	} else
		this->ContentType = "text/plain";
}

void Response::SetContentLength(std::string RequestURI) {
	std::ifstream RequestedFile(RequestURI.c_str(), std::ios::binary);
	if (!RequestedFile)
		throw Error::Forbidden403();
	std::string content((std::istreambuf_iterator<char>(RequestedFile)),
						std::istreambuf_iterator<char>());
	this->body = content;
	std::stringstream num;
	num << this->body.size();
	this->ContentLength = num.str();
}

void Response::SetVars(const std::string &RequestURI) {
	std::stringstream ss(RequestURI);
	while (getline(ss, this->filename, '/')) {
	}

	this->SetContentType();
	this->SetContentLength(RequestURI);

	this->header = "HTTP/1.1 200 ok\r\nContent-Type: ";
	this->header += this->ContentType;
	this->header += "\r\nContent-Length: ";
	this->header += ContentLength;
	this->header += "\r\n\r\n";
	this->header += body;
}