#include "Client.hpp"

// Getters
const map<string, string> &WSN::Client::get_request() const {
	return request_parser.get_request();
}

const string &WSN::Client::get_request(string key) {
	return request_parser[key];
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::Client::Client(int client_socket) : client_socket(client_socket), request_parser(RequestParser()) {
	std::memset(buffer, 0, MAXLINE + 1);
}

WSN::Client::Client(const WSN::Client &client) : client_socket(client.client_socket), request_parser(client.request_parser) {
}

WSN::Client &WSN::Client::operator=(const WSN::Client &client) {
	this->client_socket	 = client.client_socket;
	this->request_parser = client.request_parser;
	return *this;
}

WSN::Client::~Client() {
}
