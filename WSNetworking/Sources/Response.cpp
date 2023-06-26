#include "Response.hpp"

Response::Response() {}

Response::~Response() {}

std::string Response::GetContentType() const { return (this->ContentType); }

std::string Response::GetContentLength() const { return (this->ContentLength); }

std::string Response::GetHeader() const { return (this->header); }

Response::Response(MainClient *Client) { this->Client = Client; }

std::string Response::SetError(const std::string msg_status, std::string body_file) {
	std::stringstream ss(msg_status);
	int				  status;
	ss >> status;
	this->ContentType = "text/html";
	body_file		  = this->set_error_body(msg_status, body_file);  // set content length here
	this->header	  = "HTTP/1.1 ";
	this->header += msg_status;
	this->header += "\r\nContent-Type: ";
	this->header += ContentType;
	this->header += "\r\nContent-Length: ";
	this->header += this->ContentLength;
	this->header += "\r\nConnection: Close";
	if (status == 405) {
		this->header += "\r\nAllow:";
		for (std::vector<std::string>::const_iterator methods_itr
			 = Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_methods().begin();
			 methods_itr
			 != Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_methods().end();
			 methods_itr++) {
			this->header += " ";
			this->header += *methods_itr;
		}
	}
	this->header += "\r\n\r\n";
	return (body_file);
}

std::ostream &operator<<(std::ostream &out, const Response &obj) {
	out << "ContentType:" << obj.GetContentType() << std::endl;
	out << "ContentLength:" << obj.GetContentLength() << std::endl;
	out << "Header :" << std::endl;
	out << obj.GetHeader() << std::endl;
	return (out);
}

void Response::SetContentType() {
	PRINT_ERROR("set content type");
	PRINT_ERROR(this->filename);
	size_t start = this->filename.find_last_of('.');
	if (start != string::npos) {
		this->extention = filename.substr(start, filename.size() - 1);
		if (this->extention == ".py" || this->extention == ".php") {
			PRINT_ERROR("check access cgi");
			if (Client->get_access() == false) {
				PRINT_LONG_LINE("handle cgi");
				Client->set_is_cgi(true);
				check_cgi_location();
				Client->get_cgi()->check_extention();
				this->ContentType = "text/html";
			}
			PRINT_ERROR("wait for cgi");
			Client->get_cgi()->wait_for_child();
			this->serve_file = Client->get_cgi()->get_outfile();
			return;
		}
		this->ContentType = Client->get_content_type(this->extention);
	} else {
		PRINT_ERROR("here");
		this->ContentType = "text/html";
	}
	PRINT_ERROR("content type");
	PRINT_ERROR(ContentType);
}

void Response::SetContentLength(std::string RequestURI) {
	std::ifstream RequestedFile(RequestURI.c_str(), std::ios::binary);
	if (!RequestedFile)
		throw Error::Forbidden403();  // ! don't check here
	RequestedFile.seekg(0, std::ios::end);
	std::ifstream::pos_type size = RequestedFile.tellg();
	RequestedFile.seekg(0, std::ios::beg);
	std::stringstream num;
	num << size;
	this->ContentLength = num.str();
	RequestedFile.close();
}

void Response::SetVars() {
	if (Client->get_access() == false) {
		std::stringstream ss(serve_file);

		while (getline(ss, this->filename, '/')) {
		}
	}
	this->SetContentType();
	PRINT_ERROR(this->ContentType);
	if (this->extention == ".php") {
		PRINT_ERROR("handle php");
		handle_php();
		Client->set_header(header);
		SHOW_INFO(this->header);
		return;
	}
	this->SetContentLength(serve_file);
	this->header = "HTTP/1.1 200 ok\r\nContent-Type: ";
	this->header += this->ContentType;
	this->header += "\r\nContent-Length: ";
	this->header += ContentLength;
	this->header += "\r\nConnection: Close";
	this->header += "\r\n\r\n";
	Client->set_header(header);
}

//! check if it's not a valid

void Response::check_request_uri() {
	std::string root = Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_root();
	std::string uri	 = Client->get_new_url();
	PRINT_ERROR(root);
	PRINT_ERROR(uri);
	if (uri[uri.size() - 1] == '/')
		uri.erase(uri.size() - 1, 1);
	if (root == Client->get_new_url() || root == uri) {
		if (access(root.c_str(), R_OK) < 0)
			throw Error::Forbidden403();
		this->type = "directory";
		return;
	}
	if (root == "/") {
		throw_accurate_response(uri);
		return;
	}
	this->check_inside_root(root, uri);
	if (this->type.size() == 0) {
		throw Error::NotFound404();
	}
}

void Response::check_inside_root(std::string &root, std::string uri) {
	DIR *directory = opendir(root.c_str());
	if (!directory) {
		return;
	}
	dirent *list;
	while ((list = readdir(directory))) {
		std::string name = list->d_name;
		if (list->d_type == DT_DIR) {
			if (name != "." && name != "..") {
				std::string new_path = root + '/' + list->d_name;
				if (new_path == uri) {
					this->type = "directory";
					break;
				}
				check_inside_root(new_path, uri);
			}
		} else if (list->d_type == DT_REG) {
			std::string filename = root + '/' + list->d_name;
			if (filename == uri) {
				this->type = "file";
				break;
			}
		}
	}
	closedir(directory);
}

std::string Response::check_auto_index() {
	std::string filename;
	int			autoindex
		= this->Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_autoindex();
	std::string root
		= this->Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_root();
	if (autoindex == 0)
		throw Error::Forbidden403();
	else {
		DIR *directory = opendir(Client->get_new_url().c_str());
		if (!directory)
			throw Error::Forbidden403();
		filename = Client->write_into_file(directory, root);
		closedir(directory);
	}
	return (filename);
}

std::string Response::handle_directory(int flag) {
	PRINT_SHORT_LINE("handle directory");
	std::string uri = Client->get_new_url();
	if (uri[uri.size() - 1] != '/'
		&& Client->get_request("Request-URI").size() != 1)	// redirect from /folder to /folder/
	{
		std::string red = Client->get_request("Request-URI") + '/';	 // ? i should use old uri with location one
		Client->set_redirection(red);
		throw Accurate::MovedPermanently301();
	}
	std::vector<std::string> index_vec
		= Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_index();
	if (index_vec.size() != 0) {
		std::string root
			= Client->get_config_server()->get_config_location_parser()[Client->get_location()]->get_root();
		for (std::vector<std::string>::iterator itr_index = index_vec.begin(); itr_index != index_vec.end();
			 itr_index++) {
			std::string	  index_file = root + '/' + (*itr_index);
			std::ifstream file(index_file.c_str());
			if (!file.is_open())
				continue;
			else {
				file.close();
				return (index_file);
			}
		}
	}
	if (flag)
		throw Error::Forbidden403();
	return (check_auto_index());
}

std::string Response::handle_file() {
	std::ifstream file(Client->get_new_url().c_str());
	if (!file)
		throw Error::Forbidden403();
	return (Client->get_new_url());
}

std::string Response::set_error_body(std::string msg_status, std::string body_file) {
	std::string		  content;
	std::stringstream num;
	std::string		  error_file;

	if (body_file.size() == 0) {
		error_file = Client->generate_random_name();
		std::ofstream file(error_file.c_str());
		if (!file.is_open()) {
			PRINT_ERROR("HERE");
			throw Error::InternalServerError500();
		}
		content = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>";
		content += msg_status;
		content += "</title>\r\n<style>\r\nbody {\r\ntext-align: center;\r\npadding: 40px;\r\nfont-family: Arial, "
				   "sans-serif;\r\n}\r\n";
		content += "h1 {\r\nfont-size: 100px;\r\ncolor: red;\r\n}\r\n";
		content += "</style>\r\n</head>\r\n<body>\r\n<h1>";
		content += msg_status;
		content += "</body>\r\n</html>";
		file << content;
		num << content.size();
		num >> this->ContentLength;
		PRINT_ERROR("*************");
		PRINT_ERROR(error_file);
		Client->set_files_to_remove(error_file);
		file.close();
		return (error_file);
	} else {
		std::ifstream file(body_file.c_str(), std::ios::binary);
		file.seekg(0, std::ios::end);
		std::ifstream::pos_type size = file.tellg();
		file.seekg(0, std::ios::beg);
		file.close();
		num << size;
		num >> this->ContentLength;
		file.close();
	}
	return (body_file);
}

void Response::check_cgi_location() {
	if (!Client->get_config_server()
			 ->get_config_location_parser()[Client->get_location()]
			 ->get_cgi_ext_path(this->extention)
			 .size())
		throw Error::InternalServerError500();
}

void Response::set_outfile_cgi(std::string outfile) { this->cgi_outfile = outfile; }

std::string Response::Get() {
	PRINT_LONG_LINE("Handle GET");
	if (Client->get_access() == false) {
		if (Client->get_serve_file().size() == 0) {
			PRINT_ERROR("check_request_uri");
			this->check_request_uri();	// * check if uri exist in the root
			if (this->type == "directory") {
				PRINT_ERROR("should be here");
				this->serve_file = handle_directory(0);
				Client->set_new_url(this->serve_file);
			} else if (this->type == "file")
				this->serve_file = handle_file();
		} else
			serve_file = Client->get_serve_file();
	}
	PRINT_ERROR("set vars");
	this->SetVars();
	return (serve_file);
}

void Response::handle_php() {
	std::ifstream php_file(serve_file.c_str());

	php_file.seekg(0, std::ios::end);
	std::ifstream::pos_type size = php_file.tellg();
	php_file.seekg(0, std::ios::beg);
	PRINT_ERROR(size);
	char buff[MAXLINE];

	php_file.read(buff, MAXLINE);
	std::string content(buff, php_file.gcount());
	size_t		found = content.find("\r\n\r\n");
	if (found != std::string::npos) {
		PRINT_ERROR(serve_file);
		this->header = "HTTP/1.1 200 ok\r\n";
		content		 = content.substr(0, found);
		this->header += content;
		long			  len = (long)size - found;
		std::stringstream len_str;
		len_str << len;
		len_str >> this->ContentLength;
		this->header += "\r\nContentLength: ";
		this->header += this->ContentLength;
		this->header += "\r\n\r\n";
		Client->set_start_php(found + 4);
		php_file.close();
		// 	//! cgi = 1 here please
	}
}

std::string Response::post() {
	PRINT_LONG_LINE("handle post");
	if (Client->get_upload_path().size() != 0) {
		move_the_body();
		throw Accurate::Created201();
	}
	check_request_uri();
	if (this->type == "directory") {
		this->serve_file = handle_directory(1);
		Client->set_new_url(this->serve_file);
	} else
		this->serve_file = handle_file();
	this->SetVars();
	Client->set_files_to_remove(Client->get_body_file_name());
	return (serve_file);
}

void Response::move_the_body() {
	std::string path = Client->get_body_file_name();

	size_t found = path.find_last_of('/');
	if (found != std::string::npos)
		filename = path.substr(found, path.size() - found);

	this->new_path = Client->get_upload_path() + filename;
	if (std::rename(Client->get_body_file_name().c_str(), this->new_path.c_str()) != 0)
		throw Error::InternalServerError500();
	Client->reset_body_file_name(this->new_path);
}

void Response::throw_accurate_response(std::string uri) {
	DIR *dir = opendir(uri.c_str());
	if (dir == NULL) {
		if (errno == EACCES)
			throw Error::Forbidden403();
		if (errno == ENOENT)
			throw Error::NotFound404();
		if (errno == ENOTDIR) {
			std::ifstream file(uri.c_str());
			if (!file) {
				if (errno == EACCES)
					throw Error::Forbidden403();
				if (errno == ENOENT)
					throw Error::NotFound404();
				else
					throw Error::InternalServerError500();
			}
			this->type = "file";
			return;
		}
	}
	this->type = "directory";
}
