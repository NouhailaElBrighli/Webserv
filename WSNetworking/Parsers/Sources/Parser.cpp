#include "Parser.hpp"

// Getters
const string &WSN::Parser::get_data() const {
	return this->data;
}

// Setters
void WSN::Parser::set_data(string &data) {
	this->data = data;
}

// Constructors and copy constructor and copy assignment operator and destructor
WSN::Parser::Parser() {
}

WSN::Parser::Parser(string &data) : data(data) {
}

WSN::Parser::Parser(const Parser &parser) : data(parser.data) {
}

WSN::Parser &WSN::Parser::operator=(const Parser &parser) {
	this->data = parser.data;
	return *this;
}

WSN::Parser::~Parser() {
}