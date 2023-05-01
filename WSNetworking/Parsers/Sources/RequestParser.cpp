#include "RequestParser.hpp"

// data :
// GET /image.jpg HTTP/1.1
// User-Agent: PostmanRuntime/7.31.3
// Accept: */*
// Postman-Token: a85095a1-a10f-4b75-9b83-fc61f18d692f
// Host: localhost:18000
// Accept-Encoding: gzip, deflate, br
// Connection: keep-alive
// Content-Type: multipart/form-data; boundary=--------------------------224007463543657981304300
// Content-Length: 1640

// ----------------------------224007463543657981304300
// Content-Disposition: form-data; name="file"; filename="image.jpg"
// Content-Type: text/x-c
// "binary data"
// ----------------------------224007463543657981304300--

// Getters
const string &WSN::RequestParser::get_data() const {
	return this->data;
}

const map<string, string> &WSN::RequestParser::get_request() const {
	return this->request;
}

const string &WSN::RequestParser::get_request(string key) {
	return this->request[key];
}

// Setters
void WSN::RequestParser::set_data(string &data) {
	this->data = data;
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::RequestParser::RequestParser() {
}

WSN::RequestParser::RequestParser(string &data) : data(data) {
	this->parse_head();
}

WSN::RequestParser::RequestParser(const RequestParser &requestParser) : data(requestParser.data), request(requestParser.request) {
}

WSN::RequestParser &WSN::RequestParser::operator=(const RequestParser &requestParser) {
	this->data	  = requestParser.data;
	this->request = requestParser.request;
	return *this;
}

WSN::RequestParser::~RequestParser() {
}

// Methods
void WSN::RequestParser::run_head(string &data) {
	this->request.clear();
	this->data.clear();
	this->set_data(data);
	this->parse_head();
}

void WSN::RequestParser::run_body(string &data) {}

void WSN::RequestParser::parse_head() {
	this->is_data_valid();
	this->parse_first_line();
	this->is_first_line_valid();
	this->parse_rest_lines();
}

void WSN::RequestParser::is_data_valid() {
	cout << endl
		 << C_GREEN << "data.length() : " << data.length() << C_RES << endl
		 << endl;
	if (data.empty())
		throw WSN::Error::BadRequest400();

	if (data.length() > MAXLINE)
		throw WSN::Error::RequestEntityTooLarge413();
}

void WSN::RequestParser::is_first_line_valid() {
	string allowed_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";

	if (this->request.size() != 3)
		throw WSN::Error::NotImplemented501();

	if (this->request["Request-Type"] != "GET" && this->request["Request-Type"] != "POST" && this->request["Request-Type"] != "DELETE")
		throw WSN::Error::BadRequest400();

	if (this->request["Request-URI"].find_first_not_of(allowed_chars) != string::npos)
		throw WSN::Error::BadRequest400();

	if (this->request["Request-URI"].length() > 2048)
		throw WSN::Error::RequestURITooLong414();
}

void WSN::RequestParser::parse_first_line() {
	string line;
	string key;
	string value;
	size_t pos;

	if ((pos = this->data.find("\r")) != string::npos) {
		line = this->data.substr(0, pos + 2);
		this->data.erase(0, pos + 2);
		if (line.empty())
			throw WSN::Error::BadRequest400();

		if ((pos = line.find(" ")) != string::npos) {
			key				   = "Request-Type";
			value			   = line.substr(0, pos);
			this->request[key] = value;
			line.erase(0, pos + 1);
		}
		if ((pos = line.find(" ")) != string::npos) {
			key				   = "Request-URI";
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

void WSN::RequestParser::parse_rest_lines() {
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