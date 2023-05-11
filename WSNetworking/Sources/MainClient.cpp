#include "MainClient.hpp"

#include <iomanip>
// Getters
const map<string, string> &MainClient::get_request() const {
	return request_parser->get_request();
}

const string &MainClient::get_request(string key) {
	return request_parser->get_request(key);
}

const int &MainClient::get_status() const { return status; }

const string &MainClient::get_msg_status() const { return msg_status; }

// Constructors and destructor
MainClient::MainClient() { std::memset(buffer, 0, MAXLINE + 1); }

MainClient::MainClient(int				   client_socket,
					   ConfigServerParser *config_server_parser)
	: config_server_parser(config_server_parser),
	  request_parser(new RequestParser()), status(true),
	  msg_status(Accurate::OK200().what()), client_socket(client_socket) {
	std::memset(buffer, 0, MAXLINE + 1);
	try {
		this->handle(client_socket);
	} catch (const std::exception &e) {
		this->msg_status = e.what();
		this->status	 = atoi(string(e.what()).substr(0, 3).c_str());
		print_error(this->msg_status);
		print_error(this->status);
	}
}

MainClient::~MainClient() { delete request_parser; }

// Methods
void MainClient::handle(int client_socket) {
	int	   n;
	string data;
	string head;
	string body;

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
	//! body need to be fill in external file
	body = data.substr(data.find("\r\n\r\n") + 4);
	if (body.length() > this->config_server_parser->get_client_max_body_size())
		throw Error::RequestEntityTooLarge413();

	this->request_parser->run_head(head);
	// if (body.length() > 0)
	// 	this->request_parser->run_body(body);

	cout << "data : " << endl << data << endl;
	cout << "head : " << endl << head << endl;
	cout << "body : " << endl << body << endl;
	print_line("Request Parser");
	// cout << *this->request_parser << endl;

	get_matched_location_for_request_uri();
	is_method_allowded_in_location();

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
	struct stat file_info;

	if (stat(this->get_request("Request-URI").c_str(), &file_info) != 0)
		// Failed to stat file
		throw Error::NotFound404();

	if (S_ISDIR(file_info.st_mode))
		// File is a directory
		throw Error::NotFound404();

	if (S_ISREG(file_info.st_mode)) {
		// get file name to compare with index
		string file_name = this->get_request("Request-URI");
		bool   is_found	 = false;
		for (vector<ConfigLocationParser *>::const_iterator it
			 = config_server_parser->get_config_location_parser().begin();
			 it != config_server_parser->get_config_location_parser().end();
			 it++) {
			if (this->get_request("Request-URI").find((*it)->get_location())
				!= string::npos)
				file_name.erase(0, (*it)->get_location().length()),
					is_found = true;

			else if (this->get_request("Request-URI").find((*it)->get_root())
					 != string::npos)
				file_name.erase(0, (*it)->get_root().length()), is_found = true;
			if (is_found) {
				if (file_name[0] == '/')
					file_name.erase(0, 1);

				cout << "file_name : " << file_name << endl;
				for (size_t i = 0; i < (*it)->get_index().size(); i++) {
					if (file_name == (*it)->get_index(i))
						return;
				}
			}
		}
	}

	// File is not a regular file or not match with index
	throw Error::NotFound404();
}

void MainClient::is_method_allowded_in_location() {
	for (vector<ConfigLocationParser *>::const_iterator it
		 = config_server_parser->get_config_location_parser().begin();
		 it != config_server_parser->get_config_location_parser().end(); it++) {
		if (this->get_request("Request-URI").find((*it)->get_location())
			!= string::npos) {
			for (size_t i = 0; i < (*it)->get_methods().size(); i++) {
				if ((*it)->get_methods(i) == this->get_request("Request-Type"))
					return;
			}
			throw Error::MethodNotAllowed405();
		}
	}
	throw Error::MethodNotAllowed405();
}
