#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "WSNetworking.hpp"

class RequestParser {

  private:
	string head;
	bool   parse_status;
	// the first line splited to three parts : the `Request-Type`, the
	// `Request-URI`, and the `Protocol-Version`
	map<string, string> request;
	// parse body head : could be a json or a form data or a binary data

  public:
	// Getters
	const string			  &get_head() const;
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);

	// Setters
	void set_head(const string &head);
	void set_request_uri(string &str);

	// Constructors and destructor
	RequestParser();
	~RequestParser();

	// Methods
	void run_parse(const string &head);

  private:
	// Methods
	void parse_head();
	void is_head_valid();
	void is_first_line_valid();
	void parse_first_line();
	void parse_rest_lines();
	void last_check();
};

// Operator <<
std::ostream &operator<<(std::ostream &out, const RequestParser &requestParser);

#endif	// REQUESTPARSER_HPP