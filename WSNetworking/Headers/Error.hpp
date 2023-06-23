#ifndef ERROR_HPP
#define ERROR_HPP

#include "WSNetworking.hpp"

class Error {

  public:
	// Nested classes

	// 4xx Client errors
	class BadRequest400 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "400 Bad request"; }
	};

	class Unauthorized401 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "401 Unauthorized"; }
	};

	class Forbidden403 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "403 Forbidden"; }
	};

	class NotFound404 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "404 Not found"; }
	};

	class MethodNotAllowed405 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "405 Method not allowed"; }
	};

	class NotAcceptable406 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "406 Not acceptable"; }
	};

	class Conflict409 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "409 Conflict"; }
	};

	class Gone410 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "410 Gone"; }
	};

	class PreconditionFailed412 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "412 Precondition failed"; }
	};

	class RequestEntityTooLarge413 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "413 Request Entity Too Large"; }
	};

	class RequestURITooLong414 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "414 Request-URI too long"; }
	};

	class UnsupportedMediaType415 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "415 Unsupported media type"; }
	};

	// 5xx Server errors
	class InternalServerError500 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "500 Internal server error"; }
	};

	class NotImplemented501 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "501 Not implemented"; }
	};

	class ServiceUnavailable503 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "503 Service unavailable"; }
	};

	class LoopDetected508 : public std::exception {
	  public:
		virtual const char *what() const throw() { return "508 Loop Detected"; }
	};
};

#endif	// ERROR_HPP