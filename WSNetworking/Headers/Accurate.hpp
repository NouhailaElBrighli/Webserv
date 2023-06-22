#ifndef ACCURATE_HPP
#define ACCURATE_HPP

#include "WSNetworking.hpp"

class Accurate {

  public:
	// Nested classes

	// 2xx Success
	class OK200 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "200 OK"; }
	};

	class Created201 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "201 Created"; }
	};

	class Accepted202 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "202 Accepted"; }
	};

	class NoContent204 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "204 No content"; }
	};

	// 3xx Redirection
	class MultipleChoices300 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "300 Multiple choices"; }
	};

	class MovedPermanently301 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "301 Moved permanently"; }
	};

	class TemporaryRedirect302 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "302 Temporary Redirect"; }
	};

	class SeeOther303 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "303 See other"; }
	};

	class NotModified304 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "304 Not modified"; }
	};
};

#endif	// ACCURATE_HPP