#include "MainClient.hpp"

// Getters
const map<string, string> &MainClient::get_request() const { return request_parser->get_request(); }

const string &MainClient::get_request(string key) { return request_parser->get_request(key); }

const bool &MainClient::get_send_receive_status() const { return send_receive_status; }

// Constructors and destructor
MainClient::MainClient() { std::memset(buffer, 0, MAXLINE + 1); }

MainClient::MainClient(int client_socket, ConfigServerParser *config_server_parser, int port,
					   bool server_parser_set)
	: config_server_parser(config_server_parser), request_parser(new RequestParser()), status(200),
	  send_receive_status(true), msg_status(Accurate::OK200().what()), client_socket(client_socket),
	  port(port), server_parser_set(server_parser_set) {
	std::memset(buffer, 0, MAXLINE + 1);

	this->start_handle();
}

MainClient::MainClient(int client_socket, ConfigFileParser *config_file_parser, int port,
					   bool server_parser_set)
	: config_file_parser(config_file_parser), request_parser(new RequestParser()), status(200),
	  send_receive_status(true), msg_status(Accurate::OK200().what()), client_socket(client_socket),
	  port(port), server_parser_set(server_parser_set) {
	std::memset(buffer, 0, MAXLINE + 1);

	this->start_handle();
}

MainClient::~MainClient() { delete request_parser; }

// Methods
void MainClient::start_handle() {
	try {
		this->handle(this->client_socket);
	} catch (const std::exception &e) {
		this->msg_status = e.what();
		this->status	 = std::atoi(string(e.what()).substr(0, 3).c_str());
		print_error(this->msg_status);
	}
}

void MainClient::responder(int client_socket) {
	if (this->status < 400) {
		string accurate = "HTTP/1.1 ";
		accurate += this->msg_status;
		accurate += "\r\nContent-type: text/html\r\n\r\n";
		accurate += "Hello From Server\nYou are Host : ";
		accurate += this->get_request("Host") + "\r\n\r\n";
		send(client_socket, accurate.c_str(), accurate.length(), 0);
	} else {
		string error = "HTTP/1.1 ";
		error += this->msg_status;
		error += "\r\n\r\n";
		send(client_socket, error.c_str(), error.length(), 0);
	}
	this->send_receive_status = false;
}

int MainClient::get_right_server(string name_server) {
	int i = 0;
	// host:port
	string host = name_server.substr(0, name_server.find(":"));
	string port = name_server.substr(name_server.find(":") + 1, name_server.length());

	for (size_t it = 0; it < config_file_parser->get_config_server_parser().size(); it++) {
		if (config_file_parser->get_config_server_parser(it)->get_server_name() == name_server
			&& config_file_parser->get_config_server_parser(it)->get_port() == this->port)
			return i;
	}
	for (size_t it = 0; it < config_file_parser->get_config_server_parser().size(); it++) {
		if (config_file_parser->get_config_server_parser(it)->get_host() == host
			&& config_file_parser->get_config_server_parser(it)->get_port() == this->port
			&& config_file_parser->get_config_server_parser(it)->get_port_str() == port)
			return i;
		i++;
	}
	// else return the first one
	return 0;
}

void MainClient::handle(int client_socket) {
	int	   n;
	string data;
	string head;
	string body;
	int count = 0; 
	int bytes = 0;

	print_line("Client");
	while (1)
	{
		bytes = recv(client_socket, buffer, MAXLINE, 0);
		if (bytes == 0)
			break;
		if (bytes < 0)
			throw Error::BadRequest400();
		data.append(buffer, bytes);
		if (data.find("\r\n\r\n") != string::npos)
			break;
	}
	head = data.substr(0, data.find("\r\n\r\n"));
	//! BODY NEED TO BE FILL IN EXTERNAL FILE
	body = data.substr(data.find("\r\n\r\n") + 4);

	this->request_parser->run_head(head);
	cout << *this->request_parser << endl;

	// get the right config server parser if not set in constructor
	if (this->server_parser_set == false) {	 //* protected against the multiplexing
		this->server_parser_set	   = true;
		this->config_server_parser = config_file_parser->get_config_server_parser(
			get_right_server(this->get_request("Host")));
	}
	if (this->request_parser->get_request("Request-Type") != "GET")
	{
		std::cout << "head -> " << head << std::endl;
		string str = this->request_parser->get_request("Content-Length");
		std::stringstream ss(str);
		ss >> n;
		count = body.size();
		while (1 && count != n && this->request_parser->get_request("Request-Type") != "GET")
		{
			bytes = recv(client_socket, buffer, MAXLINE, 0);
			body.append(buffer, bytes);
			count += bytes;
			if (bytes < 0)
				throw Error::BadRequest400();
			if (count == n)
				break;
		}
	}

	if (body.length() > this->config_server_parser->get_client_max_body_size())
		throw Error::RequestEntityTooLarge413();

	get_matched_location_for_request_uri();
	is_method_allowded_in_location();

	this->responder(client_socket);
	this->send_receive_status = false;
	// this->responder(client_socket);
}

void MainClient::get_matched_location_for_request_uri() {
	// get file name to compare with index
	string file_name;
	bool   is_found = false;
	for (vector<ConfigLocationParser *>::const_iterator it
		 = config_server_parser->get_config_location_parser().begin();
		 it != config_server_parser->get_config_location_parser().end(); it++) {
		file_name = this->get_request("Request-URI");

		if ((*it)->get_location().find("cgi") != string::npos)
			continue;
		if (this->get_request("Request-URI").find((*it)->get_location()) != string::npos) {

			file_name.erase(0, (*it)->get_location().length());
			is_found = true;

		} else if (this->get_request("Request-URI").find((*it)->get_root()) != string::npos) {

			file_name.erase(0, (*it)->get_root().length());
			is_found = true;
		}

		if (is_found == true) {
			if (file_name[0] == '/')
				file_name.erase(0, 1);
			if (file_name.length() == 0)
				return;
			for (size_t i = 0; i < (*it)->get_index().size(); i++) {
				if (file_name == (*it)->get_index(i))
					return;
			}
		}
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
