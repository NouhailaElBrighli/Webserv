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
			return "204No content";
		}
	};

	// 4xx
	class BadRequest400 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "400Bad request";
		}
	};

	class Unauthorized401 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "401Unauthorized";
		}
	};

	class Forbidden403 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "403Forbidden";
		}
	};

	class NotFound404 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "404Not found";
		}
	};

	class MethodNotAllowed405 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "405Method not allowed";
		}
	};

	class Conflict409 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "409Conflict";
		}
	};

	class RequestEntityTooLarge413 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "413Request Entity Too Large";
		}
	};

	class RequestURITooLong414 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "414Request-URI too long";
		}
	};

	// 5xx
	class InternalServerError500 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "500Internal server error";
		}
	};

	class NotImplemented501 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "501Not implemented";
		}
	};
};

} // namespace WSN

#endif // ERROR_HPP