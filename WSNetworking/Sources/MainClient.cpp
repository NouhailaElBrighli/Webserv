#include "MainClient.hpp"

// Getters
const map<string, string> &MainClient::get_request() const { return request_parser->get_request(); }

const string &MainClient::get_request(string key) { return request_parser->get_request(key); }

const int &MainClient::get_status() const { return status; }

const string &MainClient::get_msg_status() const { return msg_status; }

const bool &MainClient::get_send_recieve_status() const { return send_recieve_status; }

// Constructors and destructor
MainClient::MainClient() { std::memset(buffer, 0, MAXLINE + 1); }

MainClient::MainClient(int client_socket, ConfigServerParser *config_server_parser, int port)
	: config_server_parser(config_server_parser), request_parser(new RequestParser()), status(200),
	  msg_status(Accurate::OK200().what()), client_socket(client_socket), port(port) {
	std::memset(buffer, 0, MAXLINE + 1);

	this->start_handle();
}

MainClient::MainClient(int client_socket, ConfigFileParser *config_file_parser, int port)
	: config_file_parser(config_file_parser), request_parser(new RequestParser()), status(200),
	  msg_status(Accurate::OK200().what()), client_socket(client_socket), port(port) {
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
	if (this->get_status() < 400) {
		string accurate = "HTTP/1.1 ";
		accurate += this->get_msg_status();
		accurate += "\r\nContent-type: text/html\r\n\r\n";
		accurate += "Hello From Server\nYou are Host : ";
		accurate += this->get_request("Host") + "\r\n\r\n";
		send(client_socket, accurate.c_str(), accurate.length(), 0);
	} else {
		string error = "HTTP/1.1 ";
		error += this->get_msg_status();
		error += "\r\n\r\n";
		send(client_socket, error.c_str(), error.length(), 0);
	}
}

int MainClient::get_right_config_server_parser_from_name_sever(string name_server) {
	int i = 0;

	name_server = name_server.substr(0, name_server.find(":"));
	for (size_t it = 0; it < config_file_parser->get_config_server_parser().size(); it++) {
		if (config_file_parser->get_config_server_parser(it)->get_server_name() == name_server)
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

	print_line("MainClient");
	while ((n = read(client_socket, buffer, MAXLINE)) > 0) {
		buffer[n] = '\0';
		data += buffer;

		if (data.find("\r\n\r\n") != string::npos)
			break;
	}

	if (n < 0) {
		print_error("Bad Request");
		throw Error::BadRequest400();
	}

	head = data.substr(0, data.find("\r\n\r\n"));

	this->request_parser->run_head(head);
	cout << *this->request_parser << endl;

	// get the right config server parser if not set in constructor
	if (this->port != -1)
		this->config_server_parser = config_file_parser->get_config_server_parser(
			get_right_config_server_parser_from_name_sever(this->get_request("Host")));

	//! body need to be fill in external file
	body = data.substr(data.find("\r\n\r\n") + 4);
	if (body.length() > this->config_server_parser->get_client_max_body_size())
		throw Error::RequestEntityTooLarge413();

	// cout << "data : " << endl << data << endl;
	// cout << "head : " << endl << head << endl;
	// cout << "body : " << endl << body << endl;
	// if (body.length() > 0)
	// 	this->request_parser->run_body(body);

	get_matched_location_for_request_uri();
	is_method_allowded_in_location();

	this->responder(client_socket);
	// if (this->get_request("Request-Type") == "GET") {
	// 	this->parse_get(reauest_pasrer->get_request());
	// } else if (this->get_request("Request-Type") == "POST") {
	// 	this->parse_post(reauest_pasrer->get_request(),
	// 					 request_parser->get_body());
	// } else if (this->get_request("Request-Type") == "DELETE") {
	// 	this->parse_delete();
	// }
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

void MainClient::is_method_allowded_in_location() {
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
