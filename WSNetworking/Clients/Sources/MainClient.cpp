#include "MainClient.hpp"

// Constructors and copy constructor and copy assignment operator and destructor

WSN::MainClient::MainClient() : WSN::Client() {
}

WSN::MainClient::MainClient(int client_socket) : WSN::Client(client_socket) {
	try {
		this->handle(client_socket);
	} catch (const std::exception &e) {
		print_error("Parse Error");
		this->status = e.what();
	}
}

WSN::MainClient::~MainClient() {
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

	// if (this->get_request("Request-Type") == "GET") {
	// 	this->parse_get();
	// } else if (this->get_request("Request-Type") == "POST") {
	// 	this->parse_post();
	// } else if (this->get_request("Request-Type") == "DELETE") {
	// 	this->parse_delete();
	// }
}