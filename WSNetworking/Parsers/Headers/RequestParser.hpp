#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "Parser.hpp"

namespace WSN {

class RequestParser : public Parser {

  private:
	// the first line splited to three parts : the `Request-Type`, the `Request-URI`, and the `Protocol-Version`
	map<string, string> request;

  public:
	// Getters
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);

	// Constructors and copy constructor and copy assignment operator and destructor
	RequestParser();
	RequestParser(string &data);
	RequestParser(const RequestParser &requestParser);
	RequestParser &operator=(const RequestParser &requestParser);
	~RequestParser();

	// Methods
	void run(string &data);

  private:
	// Methods
	void parse();
	void is_data_valid();
	void is_first_line_valid();
	void parse_first_line();
	void parse_rest();
};

// Operator <<
std::ostream &operator<<(std::ostream &out, const RequestParser &requestParser);

} // namespace WSN

#endif