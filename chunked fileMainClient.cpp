97
----------------------------676138037400800377537200
Content-Disposition: form-data; name="dfg"; filename="MainClient.cpp"
Content-Type: text/x-c


2f0e
#include "MainClient.hpp"
// Getters
const map<string, string> &MainClient::get_request() const { return request_parser->get_request(); }

const string &MainClient::get_request(string key) { return request_parser->get_request(key); }

const bool &MainClient::get_send_receive_status() const { return send_receive_status; }

const int &MainClient::get_phase() const { return phase; }

const string &MainClient::get_body_file_name() const {
	return header_body_reader->get_body_file_name();
}

const int &MainClient::get_client_socket() const { return (client_socket); }

const int &MainClient::get_location() const { return (location); }

ConfigServerParser *MainClient::get_config_server() const { return (config_server_parser); }

const map<string, string> &MainClient::get_mime_type() const { return (type_mime); }

const string &MainClient::get_mime_type(const string &type) const { return (type_mime.at(type)); }

// Setters
void MainClient::set_send_receive_status(bool send_receive_status) {
	this->send_receive_status = send_receive_status;
}

void MainClient::set_location(int location) { this->location = location; }

void MainClient::set_header(std::string header) { this->header = header; }

// Constructors and destructor
MainClient::MainClient(int client_socket, ConfigServerParser *config_server_parser)
	: config_server_parser(config_server_parser), request_parser(new RequestParser()),
	  send_receive_status(true), msg_status(Accurate::OK200().what()), client_socket(client_socket),
	  status(200), phase(READ_PHASE), header_body_reader(new HeaderBodyReader(this)) {
	std::memset(buffer, 0, MAXLINE);

	this->set_content_type_map();
	this->set_type_mime_map();
}

MainClient::~MainClient() {
	delete request_parser;
	delete header_body_reader;
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
		}

		else if (task == "write")
			this->handle_write();

	} catch (const std::exception &e) {

		if (string(e.what()).find("can't open file") != string::npos)
			throw std::runtime_error(string(e.what()));

		print_error(string(e.what()));

		if (string(e.what()) == "Still running")
			return;

		print_short_line("catch something");
		set_header_for_errors_and_redirection(e.what());

		send_to_socket();
		this->send_receive_status = false;
	}

	if (task == "write") {
		send_to_socket();
		this->send_receive_status = false;
	}
}

void MainClient::handle_read() {
	print_line("Client Request (read)");

	// 	std::memset(buffer, 0, MAXLINE);
	// 	size_t bytes = recv(this->client_socket, buffer, MAXLINE, 0);
	// 	if (bytes == 0)
	// 		return;
	// 	if (bytes < 0)
	// 		throw Error::BadRequest400();
	// 	//print buffer with specific size
	// 	std::cout << std::string(buffer, bytes);
	// 	//in case of chunk request
	//

	// 97
	// ----------------------------610700154835422642040385
	// Content-Disposition: form-data; name="dfg"; filename="MainClient.cpp"
	// Content-Type: text/x-c

	// 41a5
	// #include "MainClient.hpp"
	// .
	// .
	// .

	// 3a

	// ----------------------------610700154835422642040385--

	// 0

	//

	// 	/*
	// 	5
	// 	avc

	// 	*/
	// 	/*
	// 	de
	// 	0

	// 	*/
	// 	std::string result = chunk_reader.read(buffer, bytes);
	// 	buffer = result.c_str();
	// 	bytes = result.size();

	// 	throw std::runtime_error("Still running");
	// return ;

	header_body_reader->header_reading();
	this->request_parser->run_parse(header_body_reader->get_head());

	if (this->get_request("Request-Type") == "POST") {
		if (this->get_request("Content-Length").size() != 0)
			header_body_reader->body_reading();
		else if (this->get_request("Transfer-Encoding") == "chunked")
			header_body_reader->body_reading();
		// header_body_reader->chunked_body_reading();
		else
			throw Error::BadRequest400();
	}

	int location = this->match_location();
	if (location != -1) {
		this->location = location;
		if (this->config_server_parser->get_config_location_parser()[get_location()]
				->get_return()
				.size()
			!= 0) {
			std::string root
				= this->config_server_parser->get_config_location_parser()[get_location()]
					  ->get_root();
			std::string ret
				= this->config_server_parser->get_config_location_parser()[get_location()]
					  ->get_return();
			redirection = root + '/' + ret;
			throw Accurate::MovedPermanently301();
		}
		is_method_allowed_in_location();
	}
}

void MainClient::handle_write() {
	print_line("Server Response (write)");

	Response Response(this);
	if (this->get_request("Request-Type") == "GET") {
		serve_file = Response.Get(this);
		std::cout << "HEADER: " << this->header << std::endl;
	}
	if (this->get_request("Request-Type") == "DELETE") {
		// DELETE
	}
}

void MainClient::is_method_allowed_in_location() {
	for (vector<ConfigLocationParser *>::const_iterator it
		 = config_server_parser->get_config_location_parser().begin();
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
				str			  = this->get_request("Request-URI");
				this->new_url = this->get_request("Request-URI");
				std::string root
					= this->config_server_parser->get_config_location_parser()[locate]->get_root();
				this->new_url.erase(0, (*itr)->get_location().size());
				this->new_url
					= root + new_url;  // ? I shouldn't reset the uri for redirect it later
				return (locate);
			}
			locate++;
		}
		found = str.find_last_of('/');
		str	  = str.substr(0, found);
	}
	check_if_uri_exist();
	return (-1);
}

void MainClient::set_header_for_errors_and_redirection(const char *what) {
	this->msg_status = what;
	this->status	 = convert_to_int(this->msg_status);
	if (this->status >= 400)
		check_files_error();
	if (this->status < 400)	 // redirection
	{
		this->header = "HTTP/1.1 ";
		this->header += this->msg_status;
		this->header += "\r\nContent-Length: 0\r\n";
		this->header += "Location: ";  //? should i use port and host or not
		this->header += redirection;
		this->header += "\r\n\r\n";
	} else	// errors
	{
		Response Error;
		this->body_file = Error.SetError(msg_status, body_file);
		this->header	= Error.GetHeader();
	}
	serve_file = body_file;
}

void MainClient::set_redirection(std::string &redirection) { this->redirection = redirection; }

std::string MainClient::get_new_url() { return (this->new_url); }

std::string MainClient::get_serve_file() { return (serve_file); }

void MainClient::check_if_uri_exist() {
	DIR *directory = opendir(this->get_request("Request-URI").c_str());
	if (directory == NULL) {
		std::ifstream file(this->get_request("Request-URI"));
		if (!file.is_open())
			throw Error::NotFound404();
		file.close();
		this->serve_file = this->get_request("Request-URI");
		return;
	} else {
		if (this->get_request("Request-URI") == "/") {
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

void MainClient::check_files_error() {
	std::map<int, std::string> error_map = this->config_server_parser->get_error_page();
	if (error_map[this->status].size() != 0) {
		std::ifstream error_page(error_map[this->status]);
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
		file << '"';
		file << '>';
		file << list->d_name;
		file << "</a></li>";
	}
	file.close();
	return ("folder/serve_file.html");
}

int MainClient::convert_to_int(std::string &str) {
	int				  integer;
	std::stringstream ss(this->msg_status);
	ss >> integer;
	return (integer);
}

void MainClient::send_to_socket() {
	std::cout << "this is first header to send: " << this->header << std::endl;
	if (this->status == 301)
		send(client_socket, this->header.c_str(), header.size(), 0);
	else {
		std::ifstream file(serve_file, std::ios::binary);
		if (!file.is_open())
			throw Error::Forbidden403();
		int	 chunk = 1024;
		long count = 0;
		send(client_socket, this->header.c_str(), header.size(), 0);
		while (!file.eof()) {
			char buff[chunk];
			file.read(buff, chunk);
			count += file.gcount();
			// std::cout.write(buff, file.gcount());
			send(client_socket, buff, file.gcount(), 0);
		}
		file.close();
	}
}

void MainClient::set_content_type_map() {
	this->content_type[".txt"]	= "text/plain";
	this->content_type[".csv"]	= "text/plain";
	this->content_type[".html"] = "text/html";
	this->content_type[".htm"]	= "text/plain";
	this->content_type[".css"]	= "text/css";
	this->content_type[".jpeg"] = "image/jpeg";
	this->content_type[".jpg"]	= "image/jpeg";
	this->content_type[".png"]	= "image/png";
	this->content_type[".gif"]	= "image/gif";
	this->content_type[".ico"]	= "image/icon";
	this->content_type[".svg"]	= "image/svg+xml";
	this->content_type[".mp3"]	= "audio/mpeg";
	this->content_type[".wav"]	= "audio/wav";
	this->content_type[".mp4"]	= "video/mp4";
	this->content_type[".mov"]	= "video/quicktime";
	this->content_type[".js"]	= "application/javascript";
	this->content_type[".js"]	= "application/json";
	this->content_type[".xml"]	= "application/xml";
	this->content_type[".pdf"]	= "application/pdf";
}

void MainClient::set_type_mime_map() {
	this->type_mime["text/plain"]			  = ".txt";
	this->type_mime["text/plain"]			  = ".csv";
	this->type_mime["text/html"]			  = ".html";
	this->type_mime["text/html"]			  = ".htm";
	this->type_mime["text/css"]				  = ".css";
	this->type_mime["image/jpeg"]			  = ".jpeg";
	this->type_mime["image/jpeg"]			  = ".jpg";
	this->type_mime["image/png"]			  = ".png";
	this->type_mime["image/gif"]			  = ".gif";
	this->type_mime["image/icon"]			  = ".ico";
	this->type_mime["image/svg+xml"]		  = ".svg";
	this->type_mime["audio/mpeg"]			  = ".mp3";
	this->type_mime["audio/wav"]			  = ".wav";
	this->type_mime["video/mp4"]			  = ".mp4";
	this->type_mime["video/quicktime"]		  = ".mov";
	this->type_mime["application/javascript"] = ".js";
	this->type_mime["application/json"]		  = ".js";
	this->type_mime["application/xml"]		  = ".xml";
	this->type_mime["application/pdf"]		  = ".pdf";
}

// import os
// file_path = "./error/404.html"
// file_size = os.path.getsize(file_path)
// print("File size:", file_size, "bytes")
//

3a

----------------------------676138037400800377537200--

0

