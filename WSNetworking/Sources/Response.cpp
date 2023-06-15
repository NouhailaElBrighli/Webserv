#include "Response.hpp"

Response::Response() {}

Response::~Response() {}

std::string Response::GetContentType() const { return (this->ContentType);  }

std::string Response::GetContentLength() const { return (this->ContentLength); }

std::string Response::GetHeader() const { return (this->header); }

Response::Response(MainClient *Client) { this->Client = Client; 
this->cgi_status = 0;
}


std::string Response::SetError(const std::string msg_status, std::string body_file) {
		this->ContentType = "text/html";
		body_file = this->set_error_body(msg_status, body_file);//set content length here
		this->header = "HTTP/1.1 ";
		this->header += msg_status;
		this->header += "\r\nContent-Type: ";
		this->header += ContentType;
		this->header += "\r\nContent-Length: ";
		this->header += this->ContentLength;
		this->header += "\r\n\r\n";
		return(body_file);
}

std::ostream &operator<<(std::ostream &out, const Response &obj) {
	out << "ContentType:" << obj.GetContentType() << std::endl;
	out << "ContentLength:" << obj.GetContentLength() << std::endl;
	out << "Header :" << std::endl;
	out << obj.GetHeader() << std::endl;
	return (out);
}

void Response::SetContentType() {

	size_t start = this->filename.find('.');
	if (start != string::npos)
	{
		this->extention = filename.substr(start, filename.size() - 1);
		this->ContentType = Client->get_content_type(this->extention);
		if (ContentType == "cgi")
		{
			check_cgi_location();
			print_long_line("handle cgi");
			Cgi cgi(this->Client, Client->get_config_server()->get_config_location_parser(), Client->get_new_url());
			cgi.check_extention();
			this->serve_file = cgi.get_outfile();
			this->ContentType = "text/html";
			return;
		}
	}
	else
		this->ContentType = "application/octetstream";
}

void Response::SetContentLength(std::string RequestURI) {
	std::ifstream RequestedFile(RequestURI.c_str(), std::ios::binary);
	if (!RequestedFile)
		throw Error::Forbidden403();// ! don't check here
	RequestedFile.seekg(0, std::ios::end);
	std::ifstream::pos_type size = RequestedFile.tellg();
	RequestedFile.seekg(0, std::ios::beg);
	std::stringstream num;
	num << size;
	this->ContentLength = num.str();
	RequestedFile.close();
}

void Response::SetVars(std::string file_to_serve) {
	std::stringstream ss(serve_file);

	while (getline(ss, this->filename, '/')) {
	}
	this->SetContentType();
	if(this->extention == ".php")
	{
		print_error("here");
		handle_php();
		Client->set_header(header);
		return;
	}
	this->SetContentLength(serve_file);
	this->header = "HTTP/1.1 200 ok\r\nContent-Type: ";
	this->header += this->ContentType;
	this->header += "\r\nContent-Length: ";
	this->header += ContentLength;
	this->header += "\r\n\r\n";
	Client->set_header(header);
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

std::string	Response::check_auto_index()
{
	int autoindex =  this->Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_autoindex();
	std::string root = this->Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_root();
	if (autoindex == 0)
		throw Error::Forbidden403();
	else
	{
		DIR *directory = opendir(Client->get_new_url().c_str());
		if (!directory)
			throw Error::Forbidden403();
		Client->write_into_file(directory, root);
		closedir(directory);
	}
	return ("folder/serve_file.html");
}

std::string	Response::handle_directory()
{
	print_short_line("handle directory");
	std::string uri = Client->get_new_url();
	if (uri[uri.size() - 1] != '/' && Client->get_request("Request-URI").size() != 1)// redirect from /folder to /folder/
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
			if (!file.is_open())
				continue;
			else
			{
				file.close();
				return (index_file);
			}
		}
	}
	return (check_auto_index());
}

std::string	Response::handle_file()
{
	std::ifstream file(Client->get_new_url());
	if (!file)
		throw Error::Forbidden403();
	return (Client->get_new_url());
}

std::string	Response::set_error_body(std::string msg_status, std::string body_file)
{
	std::string content;
	std::stringstream num;
	if (body_file.size() == 0)
	{
		std::ofstream file("error/dynamic_error.html");
		if (!file.is_open())
			throw Error::BadRequest400();
		content = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>";
		content += msg_status;
		content += "</title>\r\n<style>\r\nbody {\r\ntext-align: center;\r\npadding: 40px;\r\nfont-family: Arial, sans-serif;\r\n}\r\n";
    	content += "h1 {\r\nfont-size: 100px;\r\ncolor: red;\r\n}\r\n";
		content += "</style>\r\n</head>\r\n<body>\r\n<h1>";
		content += msg_status;
		content += "</body>\r\n</html>";
		file << content;
		num << content.size();
		num >> this->ContentLength;
		return("error/dynamic_error.html");
	}
	else
	{
		std::ifstream file(body_file, std::ios::binary);
		file.seekg(0, std::ios::end);
		std::ifstream::pos_type size = file.tellg();
		file.seekg(0, std::ios::beg);
		file.close();
		num << size;
		num >> 	this->ContentLength;
		file.close();
	}
	return(body_file);
}

void	Response::check_cgi_location()
{
	if (!Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_cgi_ext_path(this->extention).size())
		throw Error::InternalServerError500();
}

void	Response::set_outfile_cgi(std::string outfile)
{
	this->cgi_outfile = outfile;
}

std::string	Response::Get() {
	
	print_long_line("Handle GET");
	if (Client->get_serve_file().size() == 0)
	{
		this->check_request_uri();// * check if uri exist in the root
		if (this->type == "directory")
			this->serve_file = handle_directory();
		else if (this->type == "file")
			this->serve_file = handle_file();
	}
	else
		serve_file = Client->get_serve_file();
	this->SetVars(serve_file);
	return (serve_file);
}

void	Response::handle_php()
{
	std::ifstream php_file(serve_file.c_str(), std::ios::binary);

	php_file.seekg(0, std::ios::end);
	std::ifstream::pos_type size = php_file.tellg();
	php_file.seekg(0, std::ios::beg);
	char buff[MAXLINE];

	php_file.read(buff, MAXLINE);
	std::string	content(buff);
	size_t found = content.find("\r\n\r\n");
	if (found != std::string::npos)
	{
		this->header = "HTTP/1.1 200 ok\r\n";
		content = content.substr(0, found);
		this->header += content;
		long len = (long)size - found;
		std::stringstream len_str;
		len_str << len;
		len_str >> this->ContentLength;
		this->header += "\r\nContentLength: ";
		this->header += this->ContentLength;
		this->header += "\r\n\r\n";
		Client->set_start_php(found + 4);
		//!cgi = 1 here please
	}
	
}

std::string	Response::post()
{
	if (Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_upload().size() != 0)
	{
		this->upload_path = Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_root() + Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_upload();
		std::cout << "the location support upload -> folder_path:" << upload_path << std::endl;
		DIR *upload_dir = opendir(upload_path.c_str());
		if (upload_dir == NULL)
			throw Error::InternalServerError500();
		closedir(upload_dir);
		set_extention_for_body_and_move_it();
	}
	this->header = "HTTP/1.1 ";
	this->header += "200 ok\r\n";
	this->header += "ContentType: text/html\r\n";
	this->header += "ContentLength: 3\r\n\r\n";
	Client->set_header(this->header);
	
	return ("folder/post_file.html");
}

void	Response::set_extention_for_body_and_move_it()
{
	std::string extention;
	if (Client->get_request("Content-Type").size() == 0)
		extention = ".bin";
	else 
		extention = Client->get_extention(Client->get_request("Content-Type"));
	std::string new_name  = Client->get_body_file_name() + extention;
	if (std::rename(Client->get_body_file_name().c_str(), new_name.c_str()) != 0)
		throw Error::BadRequest400();
	Client->reset_body_file_name(new_name);
	std::cout << "new_name :" << Client->get_body_file_name() << std::endl;
}