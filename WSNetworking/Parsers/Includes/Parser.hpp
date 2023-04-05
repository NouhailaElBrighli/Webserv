#ifndef PARSER_HPP
#define PARSER_HPP

#include "Header.hpp"

namespace WSN {

class Parser {

  protected:
	string data;

  public:
	// Getters
	const string &get_data() const;

	// Constructors and copy constructor and copy assignment operator and destructor
	Parser(string &data);
	Parser(const Parser &parser);
	Parser &operator=(const Parser &parser);
	virtual ~Parser();

	// Nested classes
	class InputError : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Input error";
		}
	};

	class ParseError : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Parse error";
		}
	};
};

} // namespace WSN

#endif