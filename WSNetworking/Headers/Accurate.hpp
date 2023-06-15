#ifndef ACCURATE_HPP
#define ACCURATE_HPP

#include "WSNetworking.hpp"

class Accurate {

  public:
	// Nested classes

	// 2xx Success
	class OK200 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "200OK"; }
	};

	class Created201 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "201Created"; }
	};

	class Accepted202 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "202Accepted"; }
	};

	class NoContent204 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "204No content"; }
	};

	// 3xx Redirection
	class MultipleChoices300 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "300Multiple choices"; }
	};

	class MovedPermanently301 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "301 Moved permanently"; }
	};

	class Found302 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "302Found"; }
	};

	class SeeOther303 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "303See other"; }
	};

	class NotModified304 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "304Not modified"; }
	};
};

#endif	// ACCURATE_HPP