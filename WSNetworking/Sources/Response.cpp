#include "Response.hpp"
#include <dirent.h>

Response::Response() {}

Response::~Response() {}

std::string Response::GetContentType() const { return (this->ContentType); }

std::string Response::GetContentLength() const { return (this->ContentLength); }

std::string Response::GetHeader() const { return (this->header); }

Response::Response(MainClient *Client) { this->Client = Client; }

void Response::set_resource_type() {
	std::cout << "URI ->" << Client->get_request("Request-URI") << std::endl;
	DIR *directory = opendir(Client->get_request("Request-URI").c_str());
	if (directory == NULL)
		type = "file";
	else {
		type = "directory";
		closedir(directory);
		if (Client->get_request("Request-URI")[Client->get_request("Request-URI").size() - 1]
			!= '/') {
			std::cout << "yes" << std::endl;
		}
	}
}

void Response::Get(MainClient *client) {
	print_long_line("Handle GET");
	this->check_request_uri(client);  // * check if uri exist in the root
	this->SetVars(client->get_request("Request-URI"));
	if (this->ContentType == "cgi")
		return;
	send(client->get_client_socket(), header.c_str(), header.size(), 0);
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
			this->ContentType = "cgi";

	} else
		this->ContentType = "text/plain";
}

void Response::SetContentLength(std::string RequestURI) {
	std::ifstream RequestedFile(RequestURI.c_str(), std::ios::binary);
	if (!RequestedFile)
		throw Error::Forbidden403();  // ! don't check here
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
	if (this->ContentType == "cgi")
		return;
	this->SetContentLength(RequestURI);

	this->header = "HTTP/1.1 200 ok\r\nContent-Type: ";
	this->header += this->ContentType;
	this->header += "\r\nContent-Length: ";
	this->header += ContentLength;
	this->header += "\r\n\r\n";
	this->header += body;
}

void Response::check_request_uri(MainClient *client) {
	int			flag = 0;
	std::string root = client->get_config_server()
						   ->get_config_location_parser()[client->get_location()]
						   ->get_root();
	std::string uri	 = client->get_request("Request-URI");
	std::string path = uri.substr(root.size() + 1);
	std::cout << "---------->the root: " << root << std::endl;
	std::cout << "--------------->uri: " << uri << std::endl;
	std::cout << "------------>: " << path << std::endl;
	// std::string new_path = path;
	// new_path = new_path.substr(1, new_path.size()); // the / in the begin
	// size_t found = new_path.find('/');
	// if (found != std::string::npos)
	// 	new_path = new_path.substr(0, found);
	// std::cout << "found:" << found << std::endl;
	// std::cout << "new_path :" << new_path << std::endl;
	DIR *root_directory = opendir(root.c_str());
	if (!root_directory)
		throw Error::BadRequest400();  // why it will fail to open the directory
	dirent *list;
	while ((list = readdir(root_directory))) {
		if (list->d_name == path) {
			flag = 1;
			std::cout << "i found it" << std::endl;
			break;
		}
		std::cout << list->d_name << std::endl;
	}
	if (!flag)
		throw Error::NotFound404();
}