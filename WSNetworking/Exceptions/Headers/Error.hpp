#ifndef ERROR_HPP
#define ERROR_HPP

#include "Include.hpp"

namespace WSN {

class Error {

  public:
	// Nested classes

	// 2xx
	class NoContent204 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "No content";
		}
	};

	// 3xx
	class MovedPermanently301 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Moved permanently";
		}
	};

	// 4xx
	class BadRequest400 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Bad request";
		}
	};

	class Unauthorized401 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Unauthorized";
		}
	};

	class Forbidden403 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Forbidden";
		}
	};

	class NotFound404 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Not found";
		}
	};

	class MethodNotAllowed405 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Method not allowed";
		}
	};

	class Conflict409 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Conflict";
		}
	};

	class RequestEntityTooLarge413 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Request entity too large";
		}
	};

	class RequestURITooLong414 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Request-URI too long";
		}
	};

	// 5xx
	class InternalServerError500 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Internal server error";
		}
	};

	class NotImplemented501 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Not implemented";
		}
	};
};

} // namespace WSN

#endif // ERROR_HPP