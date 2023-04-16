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
			return "OK";
		}
	};

	class Created201 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Created";
		}
	};

	class Accepted202 : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Accepted";
		}
	};
};

} // namespace WSN

#endif // ACCURATE_HPP