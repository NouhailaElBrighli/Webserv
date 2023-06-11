#include "MainClient.hpp"
// Getters
const map<string, string> &MainClient::get_request() const { return request_parser->get_request(); }

const string &MainClient::get_request(string key) { return request_parser->get_request(key); }

const bool &MainClient::get_send_receive_status() const { return send_receive_status; }

// Constructors and destructor
MainClient::MainClient() { std::memset(buffer, 0, MAXLINE + 1); }

MainClient::MainClient(int client_socket, ConfigServerParser *config_server_parser)
	: config_server_parser(config_server_parser), request_parser(new RequestParser()), status(200),
	  send_receive_status(true), msg_status(Accurate::OK200().what()),
	  client_socket(client_socket) {
	std::memset(buffer, 0, MAXLINE + 1);

	this->start_handle();
}

MainClient::~MainClient() { delete request_parser; }

// Methods
void MainClient::start_handle() {
	try {
		this->handle(this->client_socket);
		Response Response(this);
		if (this->request_parser->get_request("Request-Type") == "GET") {
			serve_file = Response.Get(this);
		}
	} catch (const std::exception &e) {
		print_short_line("catch something");
		set_header_for_errors_and_redirection(e.what());
	}
	send_to_socket();
	this->send_receive_status = false;
}

std::string MainClient::Header_reading(int client_socket) {
	int			bytes;
	std::string data;

	while (1) {
		bytes = recv(client_socket, buffer, MAXLINE, 0);
		buffer[bytes] = '\0';
		if (bytes == 0)
			break;
		if (bytes < 0)
			throw Error::BadRequest400();
		data.append(buffer, bytes);
		if (data.find("\r\n\r\n") != string::npos)
			break;
	}
	return (data);
}

std::string &MainClient::Body_reading(int client_socket, std::string &body) {
	int				  n, bytes, count;
	string			  str = this->request_parser->get_request("Content-Length");
	std::stringstream ss(str);
	ss >> n;
	count = body.size();
	while (1 && count != n) {
		bytes = recv(client_socket, buffer, MAXLINE, 0);
		buffer[bytes] = '\0';
		body.append(buffer, bytes);
		count += bytes;
		std::cout << buffer <<std::endl;
		if (bytes < 0)
			throw Error::BadRequest400();
		if (count == n)
			break;
	}
	return (body);
}

void	MainClient::handle(int client_socket) {
	string	data;
	string	head;
	string	body;

	print_line("Client");
	data = this->Header_reading(client_socket);
	head = data.substr(0, data.find("\r\n\r\n"));
	this->request_parser->run_parse(head);
	std::cout << *this->request_parser << std::endl;
	std::cout << "URI: " << this->get_request("Request-URI")<< std::endl;
	if (this->get_request("Transfer-Encoding").size() != 0 && this->get_request("Transfer-Encoding") != "chunked")
		throw Error::NotImplemented501();// transfer encoding exist and different to chunked
	if (this->get_request("Content-Length").size() == 0 && this->get_request("Transfer-Encoding").size() == 0 && this->get_request("Request-Type") == "POST")
		throw Error::BadRequest400();//post without content-length or transfer encoding
	if (this->request_parser->get_request("Request-Type") == "POST" && this->get_request("Content-Length").size() != 0)
	{
		body = data.substr(data.find("\r\n\r\n") + 4);
		this->Body_reading(client_socket, body);
	}
	int location = this->match_location();
	if (location != -1)
	{
		this->location = location;
		if (this->config_server_parser->get_config_location_parser()[get_location()]->get_return().size() != 0)
		{
			std::string root = this->config_server_parser->get_config_location_parser()[get_location()]->get_root();
			std::string ret = this->config_server_parser->get_config_location_parser()[get_location()]->get_return();
			redirection = root + '/' + ret;
			throw Accurate::MovedPermanently301();
		}
		is_method_allowed_in_location();
	}
}

void MainClient::is_method_allowed_in_location() {
	for (vector<ConfigLocationParser *>::const_iterator it = config_server_parser->get_config_location_parser().begin();
		 it != config_server_parser->get_config_location_parser().end(); it++) {
		if (this->get_request("Request-URI").find((*it)->get_location()) != string::npos || this->get_request("Request-URI").find((*it)->get_root()) != string::npos) {
			for (size_t i = 0; i < (*it)->get_methods().size(); i++) {
				if ((*it)->get_methods(i) == this->get_request("Request-Type"))
					return;
			}
		}
	}
	throw Error::MethodNotAllowed405();
}

int	MainClient::GetClientSocket()
{
	return (client_socket);
}

int	MainClient::match_location()
{
	std::string str = this->get_request("Request-URI");
	size_t found;
	int locate = 0;
	this->new_url = this->get_request("Request-URI");
	while (str.size() != 0)
	{
		locate = 0;
		for (vector<ConfigLocationParser *>::const_iterator itr = config_server_parser->get_config_location_parser().begin();
		itr != config_server_parser->get_config_location_parser().end() ; itr++)
		{
			if ((*itr)->get_location() == str)
			{
				str = this->get_request("Request-URI");
				this->new_url = this->get_request("Request-URI");
				std::string root = this->config_server_parser->get_config_location_parser()[locate]->get_root();
				this->new_url.erase(0, (*itr)->get_location().size());
				this->new_url = root + new_url;// ? I shouldn't reset the uri for redirect it later
				return (locate);
			}
			locate++;
		}
		found = str.find_last_of('/');
		str = str.substr(0, found);
	}
	check_if_uri_exist();
	return (-1);
}

void MainClient::set_header_for_errors_and_redirection(const char *what)
{
	this->msg_status = what;
	this->status = convert_to_int(this->msg_status);
	if (this->status >= 400)
		check_files_error();
	if (this->status  < 400) // redirection
	{
		this->header = "HTTP/1.1 ";
		this->header += this->msg_status;
		this->header += "\r\nContent-Length: 0\r\n";
		this->header += "Location: "; //? should i use port and host or not 
		this->header += redirection;
		this->header += "\r\n\r\n";
	}
	else // errors
	{
		Response	Error;
		this->body_file = Error.SetError(msg_status, body_file);
		this->header = Error.GetHeader();
	}
	serve_file = body_file;
}
	
int		MainClient::get_location()
{
	return (location);
}

ConfigServerParser	*MainClient::get_config_server()
{
	return (config_server_parser);
}

void MainClient::set_redirection(std::string &redirection)
{
	this->redirection = redirection;
}

std::string MainClient::get_new_url()
{
	return(this->new_url);
}

std::string	MainClient::get_serve_file()
{
	return(serve_file);
}

void	MainClient::check_if_uri_exist()
{
	DIR *directory = opendir(this->get_request("Request-URI").c_str());
	if (directory == NULL)
	{
		std::ifstream file(this->get_request("Request-URI"));
		if (!file.is_open())
			throw Error::NotFound404();
		file.close();
		this->serve_file = this->get_request("Request-URI");
		return;
	}
	else
	{
		if (this->get_request("Request-URI") == "/")
		{
			std::ofstream file("folder/root_directory.html");
			if (!file.is_open())
				throw Error::Forbidden403();
			char current_path[MAXLINE];
			getcwd(current_path, sizeof(current_path));
			DIR *root_directory = opendir(current_path);
			if (root_directory == NULL)
				throw Error::Forbidden403();
			this->serve_file = this->write_into_file(root_directory, current_path);
			closedir(root_directory);
			return;
		}
	}
	throw Error::NotFound404();
}

void	MainClient::check_files_error()
{
	std::map<int, std::string>error_map = this->config_server_parser->get_error_page();
	if (error_map[this->status].size() != 0)
	{
		std::ifstream error_page(error_map[this->status]);
		if (!error_page.is_open())
			throw Error::Forbidden403();
		body_file = error_map[this->status];
		error_page.close();
	}
}

std::string	MainClient::write_into_file(DIR *directory, std::string root)
{
	std::ofstream file("folder/serve_file.html");
	if (!file.is_open())
		throw Error::BadRequest400();
	file << "<!DOCTYPE html>\n<html>\n<head>\n<title>index of";
	file << root;
	file << "</title>\n<style>\nbody {\ntext-align: left;\npadding: 40px;\nfont-family: Arial, sans-serif;\n}\nh1 {\nfont-size: 32px;\ncolor: black;\n}\n</style>\n</head>\n<body>\n<h1>";
	file << "index of ";
	file << root;
	file << "</h1>\n";
	dirent *list;
	while ((list = readdir(directory)))
	{
		file << "<li> <a href= ";
		file << '"';
		file << list->d_name;
		file << '"';
		file << '>';
		file << list->d_name;
		file << "</a></li>";
	}
	file.close();
	return ("folder/serve_file.html");
}

int	MainClient::convert_to_int(std::string &str)
{
	int	integer;
	std::stringstream ss (this->msg_status);
	ss >> integer;
	return(integer);
}

void	MainClient::set_serve_file(std::string file_to_serve)
{
	this->serve_file = file_to_serve;
}

void	MainClient::set_header(std::string header)
{
	this->header = header;
}

void	MainClient::send_to_socket()
{
	std::cout << "this is first header to send: " << this->header << std::endl;
	if (this->status == 301)
		send(client_socket, this->header.c_str(), header.size(), 0);
	else
	{
		std::ifstream file(serve_file, std::ios::binary);
		if (!file.is_open())
			throw Error::Forbidden403();
		int chunk = 1024;
		long count = 0;
		send(client_socket, this->header.c_str(), header.size(), 0);
		while (!file.eof())
		{
			char buff[chunk];
			file.read(buff, chunk);
			count += file.gcount();
			// std::cout.write(buff, file.gcount());
			send(client_socket, buff, file.gcount(), 0);
		}
		file.close();
	}
}


void	MainClient::set_content_type_map()
{
	this->content_type[".txt"] = "text/plain";
	this->content_type[".csv"] = "text/plain";
	this->content_type[".html"] = "text/html";
	this->content_type[".htm"] = "text/plain";
	this->content_type[".css"] = "text/css";
	this->content_type[".jpeg"] = "image/jpeg";
	this->content_type[".jpg"] = "image/jpeg";
	this->content_type[".png"] = "image/png";
	this->content_type[".gif"] = "image/gif";
	this->content_type[".ico"] = "image/icon";
	this->content_type[".svg"] = "image/svg+xml";
	this->content_type[".mp3"] = "audio/mpeg";
	this->content_type[".wav"] = "audio/wav";
	this->content_type[".mp4"] = "video/mp4";
	this->content_type[".mov"] = "video/quicktime";
	this->content_type[".js"] = "application/javascript";
	this->content_type[".js"] = "application/json";
	this->content_type[".xml"] = "application/xml";
	this->content_type[".pdf"] = "application/pdf";
}


// import os
// file_path = "./error/404.html"
// file_size = os.path.getsize(file_path)
// print("File size:", file_size, "bytes")
//