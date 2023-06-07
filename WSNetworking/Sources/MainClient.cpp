#include "MainClient.hpp"
// Getters
const map<string, string> &MainClient::get_request() const { return request_parser->get_request(); }

const string &MainClient::get_request(string key) { return request_parser->get_request(key); }

const bool &MainClient::get_send_receive_status() const { return send_receive_status; }

const int &MainClient::get_phase() const { return phase; }

int MainClient::get_client_socket() { return (client_socket); }

int MainClient::get_location() { return (location); }

ConfigServerParser *MainClient::get_config_server() { return (config_server_parser); }

// Setters
void MainClient::set_location(int location) { this->location = location; }

// Constructors and destructor
MainClient::MainClient() { std::memset(buffer, 0, MAXLINE + 1); }

MainClient::MainClient(int client_socket, ConfigServerParser *config_server_parser, string task)
	: config_server_parser(config_server_parser), request_parser(new RequestParser()),
	  send_receive_status(true), msg_status(Accurate::OK200().what()), client_socket(client_socket),
	  status(200), phase(READ_PHASE), head_status(false), body_status(false) {
	std::memset(buffer, 0, MAXLINE + 1);

	this->start(task);
}

MainClient::~MainClient() { delete request_parser; }

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

		print_error(string(e.what()));
		if (string(e.what()) == "Still running")
			return;

		print_short_line("catch something");
		this->msg_status = e.what();
		set_header_for_errors_and_redirection();

		this->send_receive_status = false;
	}
	send(client_socket, this->header.c_str(), header.size(), 0);
	if (task == "write")
		this->send_receive_status = false;
}

void MainClient::Header_reading() {
	int bytes;

	if (this->head_status)
		return;

	bytes = recv(this->client_socket, buffer, MAXLINE, 0);
	if (bytes == 0)
		return;
	if (bytes < 0)
		throw Error::BadRequest400();
	this->head.append(buffer, bytes);
	if (this->head.find("\r\n\r\n") != string::npos) {
		this->body		  = this->head.substr(this->head.find("\r\n\r\n") + 4);
		this->head		  = this->head.substr(0, this->head.find("\r\n\r\n") + 4);
		this->head_status = true;
		return;
	} else
		throw std::runtime_error("Still running");
}

void MainClient::Body_reading() {
	int n, bytes, count;

	if (this->body_status)
		return;

	n	  = ConfigServerParser::stringToInt(this->request_parser->get_request("Content-Length"));
	count = body.size();
	if (n == 0)
		return;

	bytes = recv(this->client_socket, buffer, MAXLINE, 0);
	if (bytes < 0)
		throw Error::BadRequest400();

	this->body.append(buffer, bytes);
	count += bytes;

	if (count == n)
		return;

	if (this->body.find("\r\n\r\n") != string::npos) {
		this->body_status = true;
		return;
	} else
		throw std::runtime_error("Still running");
}

void MainClient::handle_read() {
	print_line("Client Request (read)");

	this->Header_reading();
	this->request_parser->run_parse(this->head);

	if (this->request_parser->get_request("Request-Type") == "POST"
		&& this->get_request("Content-Length").size() != 0) {
		this->Body_reading();

		print_line("body start:");
		cout << this->body << endl;
		print_line("body end:");
	}

	// this->location = this->get_matched_location_for_request_uri();
	this->location = this->check_and_change_request_uri();

	if (config_server_parser->get_config_location_parser()[this->location]->get_return().size()
		!= 0)
		throw Accurate::MovedPermanently301();
	is_method_allowed_in_location();

	// if (this->config_server_parser->get_config_location_parser()[locate]->get_autoindex() == 0)
	// 	throw Error::Forbidden403();
	// if (body.length() > this->config_server_parser->get_client_max_body_size())
	// 	throw Error::RequestEntityTooLarge413();
	// this->send_receive_status = false;
}

void MainClient::handle_write() {
	print_line("Server Response (write)");

	Response Response;
	if (this->request_parser->get_request("Request-Type") == "GET") {
		Response.Get(this);
		if (Response.GetContentType() == "cgi") {
			Cgi cgi(this, this->config_server_parser->get_config_location_parser());
			cgi.check_extention();
		}
	}
	if (this->request_parser->get_request("Request-Type") == "DELETE") {
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

int MainClient::get_matched_location_for_request_uri() {
	// get file name to compare with index
	int locate = 0;

	for (vector<ConfigLocationParser *>::const_iterator it
		 = config_server_parser->get_config_location_parser().begin();
		 it != config_server_parser->get_config_location_parser().end(); it++) {
		if ((*it)->get_location().find("cgi") != string::npos) {
			locate++;
			continue;
		}
		if (this->get_request("Request-URI") == (*it)->get_location()
			|| this->get_request("Request-URI") == (*it)->get_root())
			return locate;

		else if (this->get_request("Request-URI").find((*it)->get_location()) != string::npos)
			return locate;

		else if (this->get_request("Request-URI").find((*it)->get_root()) != string::npos)
			return locate;

		locate++;
	}

	// File is not a found
	throw Error::NotFound404();
}

int MainClient::check_and_change_request_uri() {
	std::string str = this->get_request("Request-URI");
	size_t		found;
	int			locate = this->get_matched_location_for_request_uri();

	found = str.find_last_of('/');
	str	  = str.substr(0, found);

	//! check condition of (locate == 0)
	if (str.size() != 0) {
		std::string new_url = this->get_request("Request-URI");
		new_url.replace(
			0, str.size(),
			this->config_server_parser->get_config_location_parser()[locate]->get_root());
		this->request_parser->set_request_uri(new_url);
		return locate;
	}

	//! check_if_uri_exist to serve it
	throw Error::NotFound404();
	return 0;
}

// int MainClient::change_request_uri() {
// 	std::string str = this->get_request("Request-URI");
// 	size_t		found;
// 	int			locate = 0;
// 	while (str.size() != 0) {
// 		locate = 0;
// 		for (vector<ConfigLocationParser *>::const_iterator itr
// 			 = config_server_parser->get_config_location_parser().begin();
// 			 itr != config_server_parser->get_config_location_parser().end(); itr++) {
// 			if ((*itr)->get_location() == str) {
// 				std::string new_url = this->get_request("Request-URI");
// 				new_url.replace(0, str.size(),
// 					this->config_server_parser->get_config_location_parser()[locate]->get_root());
// 				this->request_parser->set_request_uri(new_url);
// 				return (locate);
// 			}
// 			locate++;
// 		}
// 		found = str.find_last_of('/');
// 		str	  = str.substr(0, found);
// 	}
// 	//! check_if_uri_exist to serve it
// 	// throw Error::NotFound404();
// 	return (0);
// }

void MainClient::set_header_for_errors_and_redirection() {
	std::stringstream ss(this->msg_status);
	ss >> this->status;
	if (this->status < 400)	 // redirection
	{
		this->header = "HTTP/1.1 ";
		this->header += this->msg_status;
		this->header += "\r\nContent-Length: 0\r\n";
		this->header += "Location: /";	// should use port and host or not ?
		this->header += this->config_server_parser->get_config_location_parser()[get_location()]
							->get_return();
		this->header += "\r\n\r\n";
		std::cout << "Header of redirection:\n" << this->header << std::endl;
	} else	// errors
	{
		Response Error;
		Error.SetError(this->msg_status);
		this->header = Error.GetHeader();
		std::cout << "Header of Error:\n" << this->header << std::endl;
		this->send_receive_status = false;
	}
}
