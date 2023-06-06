#include "MainClient.hpp"
// Getters
const map<string, string> &MainClient::get_request() const { return request_parser->get_request(); }

const string &MainClient::get_request(string key) { return request_parser->get_request(key); }

const bool &MainClient::get_send_receive_status() const { return send_receive_status; }

// Constructors and destructor
MainClient::MainClient() { std::memset(buffer, 0, MAXLINE + 1); }

MainClient::MainClient(int client_socket, ConfigServerParser *config_server_parser, string task)
	: config_server_parser(config_server_parser), request_parser(new RequestParser()),
	  send_receive_status(true), msg_status(Accurate::OK200().what()),
	  client_socket(client_socket) {
	std::memset(buffer, 0, MAXLINE + 1);

	if (task == "read")
		this->start_handle_read();
	else if (task == "write")
		this->start_handle_write();
	else
		throw std::runtime_error("Unknown task");
}

MainClient::~MainClient() { delete request_parser; }

// Methods
void MainClient::start_handle_read() {
	try {
		this->handle_read(this->client_socket);
	} catch (const std::exception &e) {

		if (string(e.what()) == "Still reading")
			return;

		this->msg_status = e.what();
		print_error(this->msg_status);

		Response Error;
		Error.SetError(this->msg_status);

		send(client_socket, Error.GetHeader().c_str(), Error.GetHeader().size(), 0);
		std::cout << Error << std::endl;
	}
}

void MainClient::start_handle_write() {
	try {
		this->handle_write(this->client_socket);
	} catch (const std::exception &e) {
		if (string(e.what()) == "Still sending")
			return;

		this->msg_status = e.what();
		print_error(this->msg_status);

		Response Error;
		Error.SetError(this->msg_status);

		send(client_socket, Error.GetHeader().c_str(), Error.GetHeader().size(), 0);
		std::cout << Error << std::endl;
	}
	this->send_receive_status = false;
}

void MainClient::Header_reading(int client_socket) {
	int bytes;

	bytes = recv(client_socket, buffer, MAXLINE, 0);
	if (bytes == 0)
		return;
	if (bytes < 0)
		throw Error::BadRequest400();
	this->head.append(buffer, bytes);
	if (this->head.find("\r\n\r\n") != string::npos) {
		this->body = this->head.substr(this->head.find("\r\n\r\n") + 4);
		return;
	} else
		throw std::runtime_error("Still reading header");
}

void MainClient::Body_reading(int client_socket) {
	int n, bytes, count;

	n	  = ConfigServerParser::stringToInt(this->request_parser->get_request("Content-Length"));
	count = body.size();
	if (n == 0 || count == n)
		return;

	bytes = recv(client_socket, buffer, MAXLINE, 0);
	if (bytes < 0)
		throw Error::BadRequest400();
	body.append(buffer, bytes);
	count += bytes;
}

void MainClient::handle_read(int client_socket) {
	print_line("Client");

	this->Header_reading(client_socket);
	this->request_parser->run_parse(this->head);
	cout << *this->request_parser << endl;

	//! check_if_uri_dir

	if (this->request_parser->get_request("Request-Type") == "POST")  // !protect by status
		this->Body_reading(client_socket);

	int location = get_matched_location_for_request_uri();
	is_method_allowed_in_location();
	if (this->config_server_parser->get_config_location_parser()[location]->get_autoindex() == 0)
		throw Error::Forbidden403();
	if (body.length() > this->config_server_parser->get_client_max_body_size())
		throw Error::RequestEntityTooLarge413();
}

void MainClient::handle_write(int client_socket) {
	print_line("Server");

	Response Response;
	if (this->request_parser->get_request("Request-Type") == "GET") {
		Response.Get(this->request_parser->get_request("Request-URI"), client_socket);
		if (Response.GetContentType() == "cgi") {
			Cgi cgi(this, this->config_server_parser->get_config_location_parser());
			cgi.check_extention();
		}
	}
	if (this->request_parser->get_request("Request-Type") == "DELETE") {
		// DELETE
	}
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