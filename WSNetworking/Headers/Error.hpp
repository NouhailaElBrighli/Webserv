#ifndef ERROR_HPP
#define ERROR_HPP

#include "WSNetworking.hpp"

namespace WSN {

class Error {

  public:
	// Nested classes

	// 4xx Client errors
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

	class NotAcceptable406 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "406Not acceptable";
		}
	};

	class Conflict409 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "409Conflict";
		}
	};

	class Gone410 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "410Gone";
		}
	};

	class PreconditionFailed412 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "412Precondition failed";
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

	class UnsupportedMediaType415 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "415Unsupported media type";
		}
	};

	// 5xx Server errors
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

	class ServiceUnavailable503 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "503Service unavailable";
		}
	};
};

} // namespace WSN

#endif // ERROR_HPP