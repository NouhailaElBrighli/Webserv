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
		Response Response;
		if (this->request_parser->get_request("Request-Type") == "GET")
			Response.Get(this->request_parser->get_request("Request-URI"), client_socket);
	} catch (const std::exception &e) {

		std::cout << "Error:" << e.what() << std::endl;
		this->msg_status = e.what();
		std::stringstream ss(this->msg_status);
		ss >> this->status;
		Response Error;
		Error.SetError(this->msg_status);
		send(client_socket, Error.GetHeader().c_str(), Error.GetHeader().size(), 0);
		std::cout << Error << std::endl;
		this->send_receive_status = false;
		print_error(this->msg_status);
	}
}

std::string MainClient::Header_reading(int client_socket) {
	int			bytes;
	std::string data;

	while (1) {
		bytes = recv(client_socket, buffer, MAXLINE, 0);
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
		body.append(buffer, bytes);
		count += bytes;
		if (bytes < 0)
			throw Error::BadRequest400();
		if (count == n)
			break;
	}
	return (body);
}

void MainClient::handle(int client_socket) {
	string data;
	string head;
	string body;

	print_line("Client");
	data = this->Header_reading(client_socket);
	head = data.substr(0, data.find("\r\n\r\n"));
	//! BODY NEED TO BE FILL IN EXTERNAL FILE
	body = data.substr(data.find("\r\n\r\n") + 4);
	this->request_parser->run_parse(head);
	cout << *this->request_parser << endl;
	//! check_if_uri_dir
	// get the right config server parser if not set in constructor
	if (this->request_parser->get_request("Request-Type") == "POST")  // !protect by status
		this->Body_reading(client_socket, body);

	int locate = get_matched_location_for_request_uri();
	is_method_allowed_in_location();
	if (this->config_server_parser->get_config_location_parser()[locate]->get_autoindex() == 0)
		throw Error::Forbidden403();
	if (body.length() > this->config_server_parser->get_client_max_body_size())
		throw Error::RequestEntityTooLarge413();
	this->send_receive_status = false;
}

int MainClient::get_matched_location_for_request_uri() {
	// get file name to compare with index
	bool is_found = false;
	int	 locate	  = 0;

	for (vector<ConfigLocationParser *>::const_iterator it
		 = config_server_parser->get_config_location_parser().begin();
		 it != config_server_parser->get_config_location_parser().end(); it++) {
		file_name = this->get_request("Request-URI");

		if ((*it)->get_location().find("cgi") != string::npos) {
			locate++;
			continue;
		}
		if (this->get_request("Request-URI").find((*it)->get_location()) != string::npos)
			is_found = true;

		else if (this->get_request("Request-URI").find((*it)->get_root()) != string::npos)
			is_found = true;

		if (is_found == true) {
			return locate;
		}
		locate++;
	}

	// File is not a found
	this->msg_status = "404";
	std::stringstream ss(msg_status);
	ss >> status;
	throw Error::NotFound404();	 //! here
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
