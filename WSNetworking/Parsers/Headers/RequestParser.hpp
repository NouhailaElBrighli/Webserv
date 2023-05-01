#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "Exceptions.hpp"
#include "Include.hpp"

namespace WSN {

class RequestParser {

  private:
	string data;
	// the first line splited to three parts : the `Request-Type`, the `Request-URI`, and the `Protocol-Version`
	map<string, string> request;
	// parse body data : could be a json or a form data or a binary data

  public:
	// Getters
	const string			  &get_data() const;
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);

	// Setters
	void set_data(string &data);

	// Constructors and copy constructor and copy assignment operator and destructor
	RequestParser();
	RequestParser(string &data);
	RequestParser(const RequestParser &requestParser);
	RequestParser &operator=(const RequestParser &requestParser);
	~RequestParser();

	// Methods
	void run_head(string &data);
	void run_body(string &data);

  private:
	// Methods
	void parse_head();
	void is_data_valid();
	void is_first_line_valid();
	void parse_first_line();
	void parse_rest_lines();
};

// Operator <<
std::ostream &operator<<(std::ostream &out, const RequestParser &requestParser);

} // namespace WSN

#endif // REQUESTPARSER_HPP