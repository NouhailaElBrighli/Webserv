#include "RequestParser.hpp"

// Getters
const string &RequestParser::get_head() const { return this->head; }

const map<string, string> &RequestParser::get_request() const { return this->request; }

const string &RequestParser::get_request(string key) {
	if (this->request.find(key) != this->request.end())
		return this->request[key];
	// return safe empty string if key not found
	static string empty = "";
	return empty;
}

// Setters
void RequestParser::set_head(const string &head) { this->head = head; }

void RequestParser::set_request_uri(string &str) { this->request["Request-URI"] = str; }

// Set query string
void RequestParser::set_query_string() {
	string query_string;
	size_t pos;

	if ((pos = this->request["Request-URI"].find("?")) != string::npos) {
		query_string = this->request["Request-URI"].substr(pos + 1);
		this->request["Request-URI"].erase(pos);
		this->request["Query-String"] = query_string;
	}
}

// Constructors and destructor
RequestParser::RequestParser() : parse_status(false) {}

RequestParser::~RequestParser() {}

// Methods
void RequestParser::run_parse(const string &head) {
	if (this->parse_status == true)
		return;
	this->parse_status = true;
	this->request.clear();
	this->head.clear();
	this->set_head(head);
	this->parse_head();
	// print the request
	SHOW_INFO(*this);
}

void RequestParser::parse_head() {
	this->is_head_valid();
	this->parse_first_line();
	this->is_first_line_valid();
	this->parse_rest_lines();
	this->last_check();
	this->set_query_string();
}

void RequestParser::is_head_valid() {
	if (head.empty())
		throw Error::BadRequest400();
}

// Function to URL-decode a string
string RequestParser::urlDecode() {
	std::stringstream decoded;
	string			  str = this->request["Request-URI"];
	size_t			  len = str.length();
	size_t			  i	  = 0;

	while (i < len) {
		if (str[i] == '%' && i + 2 < len) {
			char hex[3]		 = {str[i + 1], str[i + 2], '\0'};
			char decodedChar = static_cast<char>(std::strtol(hex, NULL, 16));
			decoded << decodedChar;
			i += 3;
		} else if (str[i] == '+') {
			decoded << ' ';
			i++;
		} else {
			decoded << str[i];
			i++;
		}
	}

	return decoded.str();
}

void RequestParser::is_first_line_valid() {
	string allowed_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxy"
						   "z0123456789-._~:/?#[]@!$&'()*+,;=%";

	if (this->request.size() != 3)
		throw Error::NotImplemented501();

	// URL-decode the Request-URI
	string decodedURI = urlDecode();

	if (decodedURI.find_first_not_of(allowed_chars) != string::npos)
		throw Error::BadRequest400();

	if (decodedURI.length() > 2048)
		throw Error::RequestURITooLong414();
}

void RequestParser::parse_first_line() {
	string line;
	string key;
	string value;
	size_t pos;

	if ((pos = this->head.find("\r")) != string::npos) {
		line = this->head.substr(0, pos + 2);
		this->head.erase(0, pos + 2);
		if (line.empty())
			throw Error::BadRequest400();

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

void RequestParser::parse_rest_lines() {
	string line;
	string key;
	string value;
	size_t pos;

	while ((pos = this->head.find("\r")) != string::npos) {
		line = this->head.substr(0, pos);
		this->head.erase(0, pos + 2);
		if (line.empty()) {
			break;
		}
		if ((pos = line.find(": ")) != string::npos) {
			key				   = line.substr(0, pos);
			value			   = line.substr(pos + 2);
			this->request[key] = value;
		}
	}
	// parse the last line
	if (!this->head.empty()) {
		line = this->head;
		if ((pos = line.find(": ")) != string::npos) {
			key				   = line.substr(0, pos);
			value			   = line.substr(pos + 2);
			this->request[key] = value;
		}
	}
}

void RequestParser::last_check() {
	if (this->get_request("Transfer-Encoding").size() != 0 && this->get_request("Transfer-Encoding") != "chunked")
		throw Error::NotImplemented501();  // transfer encoding exist and different to chunked
	if (((this->get_request("Content-Length").size() == 0 && this->get_request("Transfer-Encoding").size() == 0)
		 || (this->get_request("Content-Length").size() != 0 && this->get_request("Transfer-Encoding").size() != 0))
		&& this->get_request("Request-Type") == "POST")
		throw Error::BadRequest400();  // post without content-length or transfer encoding
}

// Operators <<
std::ostream &operator<<(std::ostream &out, const RequestParser &requestParser) {
	out << C_PURPLE << "Request Parser {" << endl;
	for (map<string, string>::const_iterator it = requestParser.get_request().begin();
		 it != requestParser.get_request().end(); it++) {
		cout << "	" << C_CYAN << it->first << C_RED << " : " << C_GREEN << it->second << endl;
	}
	out << C_PURPLE << "}" << C_RES << endl;
	return out;
}
