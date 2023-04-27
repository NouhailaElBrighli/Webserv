#ifndef ACCURATE_HPP
#define ACCURATE_HPP

#include "Include.hpp"

namespace WSN {

class Accurate {

  public:
	// Nested classes

	// 2xx
	class OK200 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "200OK";
		}
	};

	class Created201 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "201Created";
		}
	};

	class Accepted202 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "202Accepted";
		}
	};

	// 3xx
	class MovedPermanently301 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "301Moved permanently";
		}
	};
};

} // namespace WSN

#endif // ACCURATE_HPP