#include "MainClient.hpp"
// Getters
const map<string, string> &MainClient::get_request() const { return request_parser->get_request(); }

const string &MainClient::get_request(string key) { return request_parser->get_request(key); }

const bool &MainClient::get_send_receive_status() const { return send_receive_status; }

const int &MainClient::get_phase() const { return phase; }

const string &MainClient::get_body_file_name() const { return header_body_reader->get_body_file_name(); }

const int &MainClient::get_client_socket() const { return (client_socket); }

const int &MainClient::get_location() const { return (location); }

ConfigServerParser *MainClient::get_config_server() const { return (config_server_parser); }

const map<string, string> &MainClient::get_mime_type() const { return (this->extention); }

const string &MainClient::get_mime_type(const string &type) const { return (this->extention.at(type)); }

// Setters
void MainClient::set_send_receive_status(bool send_receive_status) { this->send_receive_status = send_receive_status; }

void MainClient::set_location(int location) { this->location = location; }

void MainClient::set_header(std::string header) { this->header = header; }

void MainClient::reset_body_file_name(std::string new_name) { this->header_body_reader->set_body_file_name(new_name); }

// Constructors and destructor
MainClient::MainClient(int client_socket, ConfigServerParser *config_server_parser)
	: config_server_parser(config_server_parser), request_parser(new RequestParser()), send_receive_status(true),
	  msg_status(Accurate::OK200().what()), client_socket(client_socket), status(200), phase(READ_PHASE), php_status(0),
	  write_header(false), write_body(false), write_status(false), file_open(false),
	  header_body_reader(new HeaderBodyReader(this)), cgi_status(false), cgi_counter(0), is_cgi(false) {

	set_content_type_map();
	set_extention_map();
}

MainClient::~MainClient() {
	delete request_parser;
	delete header_body_reader;
	// delete cgi;
}

// Methods
void MainClient::start(string task) {
	if (task == "read" || task == "write")
		this->start_handle(task);
	else
		throw std::runtime_error("Unknown task");
}

void MainClient::start_handle(string task) {
	try {
		if (task == "read") {
			this->handle_read();
			this->phase = WRITE_PHASE;
		} else if (task == "write") {
			// Cgi cgi(this, this->get_config_server()->get_config_location_parser(), this->get_new_url());
			if (cgi_status == false)
			{
				if (this->write_status == false && this->status != 301)
					this->handle_write();
				send_to_socket();
			}
			else
			{
				if (is_cgi == true)
				{
					// sleep(3);
					std::cout << "HHHHH" << std::endl;
					//cgi_counter++;
					this->cgi->wait_for_child();
				}
			}
		}

	} catch (const std::exception &e) {
		PRINT_SHORT_LINE("catch something");
		if (string(e.what()).find("can't open file") != string::npos
			|| string(e.what()).find("Bad Input") != string::npos)
			throw std::runtime_error(string(e.what()));

		PRINT_ERROR(string(e.what()));

		if (string(e.what()) == "Still running")
			return;
		else
			this->phase = WRITE_PHASE;
		set_header_for_errors_and_redirection(e.what());
	}
}

void MainClient::handle_read() {
	PRINT_LINE("Client Request (read)");

	header_body_reader->header_reading();
	this->request_parser->run_parse(header_body_reader->get_head());
	this->location = this->match_location();

	if (this->config_server_parser->get_config_location_parser()[get_location()]->get_return().size() != 0) {
		// vector<string>::const_iterator it = this->config_server_parser->get_config_location_parser()[get_location()]->get_return().begin();
		throw_accurate_redirection();
	}
	is_method_allowed_in_location();
	if (this->get_request("Request-Type") == "POST") {
		check_upload_path();
		if (this->upload_path.size() == 0) {
			Response tmp(this);
			tmp.check_request_uri();
		}
		if (this->get_request("Content-Length").size() != 0)
			header_body_reader->body_reading();
		else if (this->get_request("Transfer-Encoding") == "chunked")
			header_body_reader->chunked_body_reading();
		else
			throw Error::BadRequest400();
	}
	cgi = new Cgi(this, this->get_config_server()->get_config_location_parser(), this->get_new_url());

}

void MainClient::handle_write() {
	PRINT_LINE("Server Response (write)");
	Response Response(this);
	if (this->get_request("Request-Type") == "GET") {
		write_status = true;
		serve_file	 = Response.Get();
	} else if (this->get_request("Request-Type") == "POST") {
		write_status = true;
		serve_file	 = Response.post();
	} else if (this->get_request("Request-Type") == "DELETE") {
		// DELETE
	}
}

void MainClient::is_method_allowed_in_location() {
	for (vector<ConfigLocationParser *>::const_iterator it = config_server_parser->get_config_location_parser().begin();
		 it != config_server_parser->get_config_location_parser().end(); it++) {
		if (this->get_request("Request-URI").find((*it)->get_location()) != string::npos
			|| this->get_request("Request-URI").find((*it)->get_root()) != string::npos) {
			for (size_t i = 0; i < (*it)->get_methods().size(); i++) {
				if ((*it)->get_methods(i) == this->get_request("Request-Type"))
					return;
			}
		}
	}
	throw Error::MethodNotAllowed405();
}

int MainClient::match_location() {
	std::string str = this->get_request("Request-URI");
	size_t		found;
	int			locate = 0;
	this->new_url	   = this->get_request("Request-URI");
	while (str.size() != 0) {
		locate = 0;
		for (vector<ConfigLocationParser *>::const_iterator itr
			 = config_server_parser->get_config_location_parser().begin();
			 itr != config_server_parser->get_config_location_parser().end(); itr++) {
			if ((*itr)->get_location() == str) {
				str				 = this->get_request("Request-URI");
				this->new_url	 = this->get_request("Request-URI");
				std::string root = this->config_server_parser->get_config_location_parser()[locate]->get_root();
				this->new_url.erase(0, (*itr)->get_location().size());
				this->new_url = root + new_url;	 // ? I shouldn't reset the uri for redirect it later
				return (locate);
			}
			locate++;
		}
		found = str.find_last_of('/');
		str	  = str.substr(0, found);
	}
	return (check_for_root_directory());
}

void MainClient::set_header_for_errors_and_redirection(const char *what) {
	this->msg_status   = what;
	this->status	   = convert_to_int(this->msg_status);
	this->write_status = true;
	if (this->status >= 400)
		check_files_error();
	if (this->status < 400 && this->status > 300)  // redirection
	{
		// redirection = "/" + redirection;
		this->header = "HTTP/1.1 ";
		this->header += this->msg_status;
		this->header += "\r\nContent-Length: 0\r\n";
		this->header += "Location: ";  //? should i use port and host or not
		this->header += redirection;
		this->header += "\r\nConnection: Close";
		this->header += "\r\n\r\n";
		std::cout << "redirection header:" << this->header << std::endl;
	} 
	
	else	// errors
	{
		Response Error;
		this->body_file = Error.SetError(msg_status, body_file);
		this->header	= Error.GetHeader();
	}
	serve_file = body_file;
	std::cout << "serve_file:" << body_file << std::endl;
}

void MainClient::set_redirection(std::string &redirection) { this->redirection = redirection; }

std::string MainClient::get_new_url() { return (this->new_url); }

std::string MainClient::get_serve_file() { return (serve_file); }

void MainClient::check_files_error() {
	std::map<int, std::string> error_map = this->config_server_parser->get_error_page();
	if (error_map[this->status].size() != 0) {
		std::ifstream error_page(error_map[this->status].c_str());
		if (!error_page.is_open())
			throw Error::Forbidden403();
		body_file = error_map[this->status];
		error_page.close();
	}
}

std::string MainClient::write_into_file(DIR *directory, std::string root) {
	std::ofstream file("folder/serve_file.html");
	if (!file.is_open())
		throw Error::BadRequest400();
	file << "<!DOCTYPE html>\n<html>\n<head>\n<title>index of";
	file << root;
	file << "</title>\n<style>\nbody {\ntext-align: left;\npadding: 40px;\nfont-family: Arial, "
			"sans-serif;\n}\nh1 {\nfont-size: 32px;\ncolor: "
			"black;\n}\n</style>\n</head>\n<body>\n<h1>";
	file << "index of ";
	file << root;
	file << "</h1>\n";
	dirent *list;
	while ((list = readdir(directory))) {

		file << "<li> <a href= ";
		file << '"';
		file << list->d_name;
		if (list->d_type != DT_REG)
			file << '/';
		file << '"';
		file << '>';
		file << list->d_name;
		if (list->d_type != DT_REG)
			file << '/';
		file << "</a></li>";
	}
	file.close();
	return ("folder/serve_file.html");
}

int MainClient::convert_to_int(const std::string &str) {
	int				  integer;
	std::stringstream ss(str);
	ss >> integer;
	return (integer);
}

void MainClient::set_content_type_map() {
	this->content_type[".txt"]	= "text/plain";
	this->content_type[".text"] = "text/plain";
	this->content_type[".csv"]	= "text/plain";
	this->content_type[".html"] = "text/html";
	this->content_type[".htm"]	= "text/plain";
	this->content_type[".css"]	= "text/css";
	this->content_type[".jpeg"] = "image/jpeg";
	this->content_type[".jpg"]	= "image/jpeg";
	this->content_type[".png"]	= "image/png";
	this->content_type[".gif"]	= "image/gif";
	this->content_type[".bmp"]	= "image/bmp";
	this->content_type[".svg"]	= "image/svg+xml";
	this->content_type[".ico"]	= "image/icon";
	this->content_type[".svg"]	= "image/svg+xml";
	this->content_type[".mp3"]	= "audio/mpeg";
	this->content_type[".wav"]	= "audio/wav";
	this->content_type[".mp4"]	= "video/mp4";
	this->content_type[".webm"] = "video/webm";
	this->content_type[".mov"]	= "video/quicktime";
	this->content_type[".js"]	= "application/javascript";
	this->content_type[".js"]	= "application/json";
	this->content_type[".xml"]	= "application/xml";
	this->content_type[".pdf"]	= "application/pdf";
	this->content_type[".zip"]	= "application/zip";
	this->content_type[".gz"]	= "application/gzip";
	this->content_type[".xls"]	= "application/vnd.ms-excel";
	this->content_type[".doc"]	= "application/msword";
	this->content_type[".docs"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	this->content_type["xls"]	= "application/vnd.ms-excel";
	this->content_type["xlsx"]	= "application/vnd.ms-excel";
}

std::string MainClient::get_content_type(std::string extention) { return (this->content_type[extention]); }

int MainClient::check_for_root_directory() {
	int location = 0;
	for (vector<ConfigLocationParser *>::const_iterator itr
		 = config_server_parser->get_config_location_parser().begin();
		 itr != config_server_parser->get_config_location_parser().end(); itr++) {
		if ((*itr)->get_location() == "/") {
			std::string root = this->config_server_parser->get_config_location_parser()[location]->get_root();
			if (root == "/")
				this->new_url = this->get_request("Request-URI");
			else
				this->new_url = root + this->get_request("Request-URI");
			return (location);
		}
		location++;
	}
	SHOW_INFO("should throw here");
	throw Error::NotFound404();
}

void MainClient::set_start_php(int start) {
	this->php_status = 1;
	this->start_php	 = start;
}

void MainClient::send_to_socket() {
	PRINT_LINE("sending");
	if (write_header == false) {
		PRINT_SHORT_LINE("send header");
		SHOW_INFO(this->header);
		send(client_socket, this->header.c_str(), header.size(), 0);
		if (this->status == 301 || this->status == 302) {
			PRINT_ERROR("close the socket now");
			this->send_receive_status = false;
			return;
		}
		write_header = true;
		return;
	}
	std::ifstream file(serve_file.c_str(), std::ios::binary);

	if (file_open == false) {
		PRINT_SHORT_LINE("open the file");
		if (!file.is_open()) {
			throw Error::Forbidden403();
		}
		if (this->php_status) {
			char buff[start_php];
			file.read(buff, start_php);
			this->position = file.tellg();
		}
		file_open = true;
		file.close();
		return;
	}
	PRINT_SHORT_LINE("sending body");
	file.seekg(position);
	if (!file.is_open())
		throw Error::BadRequest400();
	if (position == -1) {
		file.close();
		PRINT_ERROR("close the socket now");
		this->send_receive_status = false;
		remove_files();
		return;
	}
	char buff[MAXLINE];

	file.read(buff, MAXLINE);

	this->position = file.tellg();
	if (send(client_socket, buff, file.gcount(), 0) < 0)
		throw Error::BadRequest400();
	file.close();
}

void MainClient::set_extention_map() {
	this->extention["text/plain"]															   = ".txt";
	this->extention["text/html"]															   = ".html";
	this->extention["text/css"]																   = ".css";
	this->extention["image/jpeg"]															   = ".jpeg";
	this->extention["image/png"]															   = ".png";
	this->extention["image/bmp"]															   = ".bmp";
	this->extention["text/png"]																   = ".png";
	this->extention["image/svg+xml"]														   = ".svg";
	this->extention["image/icon"]															   = ".icon";
	this->extention["audio/mpeg"]															   = ".mp3";
	this->extention["audio/wav"]															   = ".wav";
	this->extention["video/mp4"]															   = ".mp4";
	this->extention["video/webm"]															   = ".webm";
	this->extention["video/quicktime"]														   = ".mov";
	this->extention["application/json"]														   = ".js";
	this->extention["application/xml"]														   = ".xml";
	this->extention["application/pdf"]														   = ".pdf";
	this->extention["application/zip"]														   = ".zip";
	this->extention["application/gzip"]														   = ".gz";
	this->extention["application/msword"]													   = ".doc";
	this->extention["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
	this->extention["application/vnd.ms-excel"]												   = ".xls";
	this->extention["application/vnd.ms-excel"]												   = ".xlsx";
	this->extention["application/x-httpd-php"]												   = ".php";
}

void MainClient::check_upload_path() {
	if (this->get_config_server()->get_config_location_parser()[this->get_location()]->get_upload().size() != 0) {
		this->upload_path = this->get_config_server()->get_config_location_parser()[this->get_location()]->get_root()
							+ this->get_config_server()->get_config_location_parser()[get_location()]->get_upload();
		DIR *directory = opendir(this->upload_path.c_str());
		if (directory == NULL) {
			std::cout << "this->upload_path" << this->upload_path << std::endl;
			throw Error::BadRequest400();  //! 500
		}
		closedir(directory);
		return;
	}
	// throw Error::InternalServerError500();
}

std::string MainClient::get_upload_path() { return (upload_path); }

void	MainClient::throw_accurate_redirection()
{
	vector<string>::const_iterator it = this->config_server_parser->get_config_location_parser()[get_location()]->get_return().begin();
	if (*it == "301")
	{
		it++;
	    redirection = *it;
		throw Accurate::MovedPermanently301();
	}
	else if (*it == "302")
	{
		it++;
		redirection = *it;
		throw Accurate::TemporaryRedirect302();
	}
	else
	{
		redirection = *it;
		throw Accurate::TemporaryRedirect302();
	}
}

void	MainClient::remove_files()
{
	std::remove("folder/s/rve_file.html");
}

void	MainClient::set_write_status(bool status)
{
	this->write_status = status;
}

void	MainClient::set_cgi_status(bool status)
{
	cgi_status = status;
}

bool MainClient::get_cgi_status()
{
	return(cgi_status);
}

int	MainClient::get_cgi_counter()
{
	return(cgi_counter);
}

Cgi *MainClient::get_cgi()
{
	return(cgi);
}

void	MainClient::set_is_cgi(bool status)
{
	this->is_cgi = status;
}