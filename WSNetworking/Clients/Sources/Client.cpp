#include "Client.hpp"

// Getters
const map<string, string> &WSN::Client::get_request() const {
	return request_parser->get_request();
}

const string &WSN::Client::get_request(string key) {
	return request_parser->get_request(key);
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::Client::Client() {
	std::memset(buffer, 0, MAXLINE + 1);
}

WSN::Client::Client(int client_socket) : request_parser(new RequestParser()), client_socket(client_socket) {
	std::memset(buffer, 0, MAXLINE + 1);
}

WSN::Client::~Client() {
	delete request_parser;
}
