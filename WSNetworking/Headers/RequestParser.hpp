#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "WSNetworking.hpp"

namespace WSN {

class RequestParser {

  private:
	string head;
	string body;
	// the first line splited to three parts : the `Request-Type`, the `Request-URI`, and the `Protocol-Version`
	map<string, string> request;
	// parse body head : could be a json or a form data or a binary data

  public:
	// Getters
	const string			  &get_head() const;
	const string			  &get_body() const;
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);

	// Setters
	void set_head(string &head);
	void set_body(string &body);

	// Constructors and copy constructor and copy assignment operator and destructor
	RequestParser();
	RequestParser(const RequestParser &requestParser);
	RequestParser &operator=(const RequestParser &requestParser);
	~RequestParser();

	// Methods
	void run_head(string &head);
	void run_body(string &head);

  private:
	// Methods
	void parse_head();
	void is_head_valid();
	void is_first_line_valid();
	void parse_first_line();
	void parse_rest_lines();
};

// Operator <<
std::ostream &operator<<(std::ostream &out, const RequestParser &requestParser);

} // namespace WSN

#endif // REQUESTPARSER_HPP