#ifndef MAINCLIENT_HPP
#define MAINCLIENT_HPP

#include "Exceptions.hpp"
#include "Include.hpp"
#include "RequestParser.hpp"

namespace WSN {

class MainClient {

  private:
	RequestParser *request_parser;
	string		   status;
	int			   client_socket;
	char		   buffer[MAXLINE + 1];

	MainClient(const MainClient &);
	MainClient &operator=(const MainClient &);

  public:
	// Getters
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);

	const string &get_status() const;

	// Constructors and copy constructor and copy assignment operator and destructor
	MainClient();
	MainClient(int client_socket);
	~MainClient();

  private:
	// Methods
	void handle(int client_socket);
	void get_matched_location_for_request_uri();
	void is_method_allowded_in_location();
};

} // namespace WSN

#endif // MAINCLIENT_HPP