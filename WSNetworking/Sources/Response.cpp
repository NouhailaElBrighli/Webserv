#include "Response.hpp"
#include <dirent.h>

Response::Response() {}

Response::~Response() {}

std::string Response::GetContentType() const { return (this->ContentType); }

std::string Response::GetContentLength() const { return (this->ContentLength); }

std::string Response::GetHeader() const { return (this->header); }

Response::Response(MainClient *Client)
{
	this->Client = Client;
}

void Response::Get(MainClient *client) {
	print_long_line("Handle GET");
	this->check_request_uri();// * check if uri exist in the root
	std::string file_to_serve;
	if (this->type == "directory")
		file_to_serve = handle_directory();
	else if (this->type == "file")
		file_to_serve = handle_file();
	this->SetVars(file_to_serve);
	if (this->ContentType == "cgi")
		return;
	send(client->GetClientSocket(), header.c_str(), header.size(), 0);
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
		throw Error::Forbidden403();// ! don't check here
	std::string content((std::istreambuf_iterator<char>(RequestedFile)), std::istreambuf_iterator<char>());
	this->body = content;
	std::stringstream num;
	num << this->body.size();
	this->ContentLength = num.str();
}

void Response::SetVars(std::string file_to_serve) {
	std::stringstream ss(file_to_serve);

	while (getline(ss, this->filename, '/')) {
	}
	this->SetContentType();
	this->SetContentLength(file_to_serve);

	this->header = "HTTP/1.1 200 ok\r\nContent-Type: ";
	this->header += this->ContentType;
	this->header += "\r\nContent-Length: ";
	this->header += ContentLength;
	this->header += "\r\n\r\n";
	this->header += body;
	// std::cout << "header :" << this->header << std::endl;
}

void	Response::check_request_uri()
{
	std::string root = Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_root();
	std::string uri = Client->get_new_url();
	if (uri[uri.size() - 1] == '/')
		uri.erase(uri.size() - 1, 1);
	if (root == uri)
	{
		this->type = "directory";
		return;
	}
	this->check_inside_root(root, uri);
	if (this->type.size() == 0)
		throw Error::NotFound404();
}

void	Response::check_inside_root(std::string &root, std::string uri)
{
	DIR *directory = opendir(root.c_str());
	if (!directory)
	{
		std::cout << "failed to open directory: " << uri << std::endl;
		throw Error::BadRequest400();
	}
	dirent *list;
	while ((list = readdir(directory)))
	{
		std::string name = list->d_name;
		if (list->d_type == DT_DIR)
		{
			if (name != "." && name != "..")
			{
				std::string new_path = root + '/' + list->d_name;
				if (new_path == uri) 
				{
					this->type = "directory";
					break;
				}
				check_inside_root(new_path, uri);
			}
		}
		else if (list->d_type == DT_REG)
		{	
			std::string filename = root + '/' + list->d_name;
			if (filename == uri)
			{
				this->type = "file";
				break;
			}
		}
	}
	closedir(directory);
}

void	Response::serve_file(std::string	index_file)
{
	std::cout << "file to serve: " << index_file << std::endl;// search for cgi here

}

std::string	Response::check_auto_index()
{
	int autoindex =  this->Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_autoindex();
	if (autoindex == 0)
		throw Error::Forbidden403();
	else
	{
		DIR *directory = opendir(Client->get_new_url().c_str());
		// std::cout << "url: " << Client->get_new_url() << std::endl;
		if (!directory)
		{
			std::cout << "failed to open the directory" << std::endl;
			throw Error::Forbidden403();
		}
		dirent *list;
		while ((list = readdir(directory)))
		{
			std::ofstream file("folder/serve_file");
			if (!file.is_open())
				throw Error::BadRequest400();
			file << "* " << list->d_name << std::endl;
			std::cout << "* " << list->d_name << std::endl;
		}
		closedir(directory);
	}
	return ("folder/serve_file");
}

std::string	Response::handle_directory()
{
	print_short_line("handle directory");
	std::string uri = Client->get_new_url();
	if (uri[uri.size() - 1] != '/')// redirect from /folder to /folder/
	{
		std::string red =  Client->get_request("Request-URI") + '/'; // ? i should use old uri with location one
		Client->set_redirection(red);
		throw Accurate::MovedPermanently301();
	}
	std::vector<std::string > index_vec =  Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_index();
	if (index_vec.size() != 0)
	{
		std::string root = Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_root();
		for (std::vector<std::string>::iterator itr_index = index_vec.begin(); itr_index != index_vec.end(); itr_index++)
		{
			std::string index_file = root + '/' + (*itr_index);
			std::ifstream file(index_file);
			if (!file)
				continue;
			else
			{
				file.close();
				return (index_file);
			}
		}
		throw Error::Forbidden403();//should i throw if the files doesn't exist
	}
	return (check_auto_index());
}

std::string	Response::handle_file()
{
	print_short_line("handle file");
	std::ifstream file(Client->get_new_url());
	if (!file)
		throw Error::Forbidden403();
	return (Client->get_new_url());
}
