#include "MainClient.hpp"

// Constructors and copy constructor and copy assignment operator and destructor
WSN::MainClient::MainClient(int client_socket) : WSN::Client(client_socket) {
	this->handle(client_socket);
}

WSN::MainClient::MainClient(const WSN::MainClient &mainClient) : WSN::Client(mainClient) {
}

WSN::MainClient &WSN::MainClient::operator=(const WSN::MainClient &mainClient) {
	WSN::Client::operator=(mainClient);
	return *this;
}

WSN::MainClient::~MainClient() {
}

// Methods
void WSN::Client::handle(int client_socket) {
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
		print_line("Read Error");
		throw WSN::Client::ReadError();
	}
	cout << data << endl;
	try {
		this->request_parser.run(data);
		print_line("Request Parser");
		cout << this->request_parser << endl;
	} catch (const std::exception &e) {
		print_line("Parse Error");
		throw WSN::RequestParser::ParseError();
	}
}