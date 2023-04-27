#include "MainClient.hpp"

// Getters
const map<string, string> &WSN::MainClient::get_request() const {
	return request_parser->get_request();
}

const string &WSN::MainClient::get_request(string key) {
	return request_parser->get_request(key);
}

const string &WSN::MainClient::get_status() const {
	return status;
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::MainClient::MainClient() {
	std::memset(buffer, 0, MAXLINE + 1);
}

WSN::MainClient::MainClient(int client_socket) : request_parser(new RequestParser()), status(WSN::Accurate::OK200().what()), client_socket(client_socket) {
	std::memset(buffer, 0, MAXLINE + 1);
	try {
		this->handle(client_socket);
	} catch (const std::exception &e) {
		print_error(e.what());
		this->status = e.what();
	}
}

WSN::MainClient::~MainClient() {
	delete request_parser;
}

// Methods
void WSN::MainClient::handle(int client_socket) {
	int	   n;
	string data;

	while ((n = read(client_socket, buffer, MAXLINE)) > 0) {
		buffer[n] = '\0';
		data += buffer;
		if (data.find("\r\n\r\n") != string::npos) {
			break;
		}
	}

	if (n < 0) {
		print_error("Bad Request");
		throw WSN::Error::BadRequest400();
	}

	cout << data << endl;

	this->request_parser->run(data);

	print_line("Request Parser");
	cout << *this->request_parser << endl;

	get_matched_location_for_request_uri();
	// is_method_allowded_in_location();//! TODO: Unable when parsing of ConfigFile is ready

	// if (this->get_request("Request-Type") == "GET") {
	// 	this->parse_get();
	// } else if (this->get_request("Request-Type") == "POST") {
	// 	this->parse_post();
	// } else if (this->get_request("Request-Type") == "DELETE") {
	// 	this->parse_delete();
	// }
}

void WSN::MainClient::get_matched_location_for_request_uri() {
	struct stat file_info;

	if (stat(this->get_request("Request-URI").c_str(), &file_info) != 0)
		// Failed to stat file
		throw WSN::Error::NotFound404();

	if (S_ISDIR(file_info.st_mode))
		// File is a directory
		throw WSN::Error::NotFound404();

	if (S_ISREG(file_info.st_mode))
		// File is a regular file
		return;

	// File is not a directory or a regular file
	throw WSN::Error::NotFound404();
}

void WSN::MainClient::is_method_allowded_in_location() {
	//! TODO: Unable when parsing of ConfigFile is ready
	// if (this->get_request("Request-Type") == WSN::ConfigFileParsing::get_instance()->get_config()["method"])
	// 	return;

	throw WSN::Error::MethodNotAllowed405();
}
