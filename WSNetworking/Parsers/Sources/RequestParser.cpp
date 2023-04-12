#include "RequestParser.hpp"

// GET / HTTP/1.1
// Host: 127.0.0.1:8080
// Connection: keep-alive
// Cache-Control: max-age=0
// sec-ch-ua: "Google Chrome";v="111", "Not(A:Brand";v="8", "Chromium";v="111"
// sec-ch-ua-mobile: ?0
// sec-ch-ua-platform: "macOS"
// DNT: 1
// Upgrade-Insecure-Requests: 1
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/111.0.0.0 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
// Sec-Fetch-Site: cross-site
// Sec-Fetch-Mode: navigate
// Sec-Fetch-Dest: document
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en,fr;q=0.9,ar;q=0.8

// Getters
const map<string, string> &WSN::RequestParser::get_request() const {
	return this->request;
}

const string &WSN::RequestParser::get_request(string key) {
	return this->request[key];
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::RequestParser::RequestParser() : Parser() {
}

WSN::RequestParser::RequestParser(string &data) : Parser(data) {
	this->run();
}

WSN::RequestParser::RequestParser(const RequestParser &requestParser) : Parser(requestParser) {
}

WSN::RequestParser &WSN::RequestParser::operator=(const RequestParser &requestParser) {
	Parser::operator=(requestParser);
	return *this;
}

WSN::RequestParser::~RequestParser() {
}

// Operators
const string &WSN::RequestParser::operator[](string key) {
	return this->request[key];
}

// Methods
void WSN::RequestParser::run() {
	this->parse_first_line();
	this->parse_rest();
}

void WSN::RequestParser::run(string &data) {
	this->request.clear();
	this->data.clear();
	this->set_data(data);
	this->parse_first_line();
	this->parse_rest();
}

void WSN::RequestParser::parse_first_line() {
	string line;
	string key;
	string value;
	size_t pos;

	if ((pos = this->data.find("\r")) != string::npos) {
		line = this->data.substr(0, pos);
		this->data.erase(0, pos + 2);
		if (line.empty()) {
			throw WSN::Parser::InputError();
		}
		if ((pos = line.find(" ")) != string::npos) {
			key				   = "Request-Type";
			value			   = line.substr(0, pos);
			this->request[key] = value;
			line.erase(0, pos + 1);
		}
		if ((pos = line.find(" ")) != string::npos) {
			key				   = "Document-Path";
			value			   = line.substr(0, pos);
			this->request[key] = value;
			line.erase(0, pos + 1);
		}
		if ((pos = line.find("\r")) != string::npos) {
			key				   = "Protocol-Version";
			value			   = line.substr(0, pos);
			this->request[key] = value;
		}
	}
}

void WSN::RequestParser::parse_rest() {
	string line;
	string key;
	string value;
	size_t pos;

	while ((pos = this->data.find("\r")) != string::npos) {
		line = this->data.substr(0, pos);
		this->data.erase(0, pos + 2);
		if (line.empty()) {
			break;
		}
		if ((pos = line.find(": ")) != string::npos) {
			key				   = line.substr(0, pos);
			value			   = line.substr(pos + 2);
			this->request[key] = value;
		}
	}
}

// Operators <<
std::ostream &WSN::operator<<(std::ostream &out, const WSN::RequestParser &requestParser) {
	out << "RequestParser {" << endl;
	out << "	request: {" << endl;
	for (map<string, string>::const_iterator it = requestParser.get_request().begin(); it != requestParser.get_request().end(); it++) {
		cout << "		" << it->first << " : " << it->second << endl;
	}
	out << "	}" << endl;
	out << "}" << endl;
	return out;
}