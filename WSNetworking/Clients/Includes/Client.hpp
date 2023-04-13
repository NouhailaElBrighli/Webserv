#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Header.hpp"
#include "RequestParser.hpp"

namespace WSN {

class Client {
  protected:
	RequestParser *request_parser;
	int			   client_socket;
	char		   buffer[MAXLINE + 1];

	Client(const Client &client);
	Client &operator=(const Client &client);

  public:
	// Getters
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);

	// Constructors and copy constructor and copy assignment operator and destructor
	Client();
	Client(int client_socket);
	virtual ~Client();

	virtual void handle(int client_socket) = 0;

	// Nested classes
	class ReadError : public std::exception {
	  public:
		virtual const char *what() const throw() {
			return "Read error";
		}
	};
};

} // namespace WSN

#endif // CLIENT_HPP