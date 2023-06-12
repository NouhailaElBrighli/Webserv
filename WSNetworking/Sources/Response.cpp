#include "Response.hpp"

Response::Response() {}

Response::~Response() {}

std::string Response::GetContentType() const { return (this->ContentType);  }

std::string Response::GetContentLength() const { return (this->ContentLength); }

std::string Response::GetHeader() const { return (this->header); }

Response::Response(MainClient *Client) { this->Client = Client; 
this->cgi_status = 0;
}

std::string	Response::Get(MainClient *client) {
	
	print_long_line("Handle GET");
	std::string file_to_serve;
	if (Client->get_serve_file().size() == 0)
	{
		this->check_request_uri();// * check if uri exist in the root
		if (this->type == "directory")
			file_to_serve = handle_directory();
		else if (this->type == "file")
			file_to_serve = handle_file(); 
	}
	else
		file_to_serve = Client->get_serve_file();
	this->SetVars(file_to_serve);
	return (file_to_serve);
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
	std::string extention;

	size_t start = this->filename.find('.');
	if (start != string::npos)
	{
		std::string extention = filename.substr(start, filename.size() - 1);
		std::cout << "extention :" << extention << std::endl;
		this->ContentType = Client->get_content_type(extention);
		print_long_line(ContentType);
		if (ContentType == "cgi")
		{
			print_error("rah dkheeeeel");
			std::string str;
			Cgi cgi(this->Client, Client->get_config_server()->get_config_location_parser(), Client->get_new_url());
			cgi.check_extention();
			outfile_cgi = cgi.get_outfile();
			std::cout << "outfile :" << outfile_cgi << std::endl;
			std::ifstream outfile(outfile_cgi);
			if(!outfile.is_open())
				throw Error::BadRequest400();
			char buff[MAXLINE];
			outfile.read(buff, MAXLINE);
			std::string fake_buff(buff);
			size_t found = fake_buff.find("\r\n\r\n");
			this->header = "HTTP/1.1 200 ok\r\n";
			if (found != std::string::npos)
			{
				this->header += fake_buff.substr(0, found + 4);
				this->body = fake_buff.substr(found + 4, fake_buff.size() - found);
				std::cout << "this->header: " << this->header << std::endl;
				std::cout << "this->body: " << this->body << std::endl;
				// serve_file << this->body;
				// std::cout << "serve_file" << serve_file << std::endl;
			}
			this->cgi_status = 1;
			return;
			//modifier content type
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
	std::stringstream ss(file_to_serve);

	while (getline(ss, this->filename, '/')) {
	}
	this->SetContentType();
	if (!this->cgi_status)
	{
		this->SetContentLength(file_to_serve);
		this->header = "HTTP/1.1 200 ok\r\nContent-Type: ";
		this->header += this->ContentType;
		this->header += "\r\nContent-Length: ";
		this->header += ContentLength;
		this->header += "\r\n\r\n";
	}
	Client->set_header(header);
}

void	Response::check_request_uri()
{
	print_error("location");
	print_error(Client->get_location());
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
	print_short_line("handle file");
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

std::string	Response::post(MainClient *Client)
{
	this->Client = Client;
	std::cout << Client->get_body_file_name() << std::endl;
	this->header = "HTTP/1.1 ";
	this->header += "200 ok\r\n";
	this->header += "ContentType: text/html\r\n";
	this->header += "ContentLength: 3\r\n\r\n";
	Client->set_header(this->header);
	return ("folder/post_file.html");
}
