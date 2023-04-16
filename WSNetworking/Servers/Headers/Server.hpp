#ifndef SERVER_HPP
#define SERVER_HPP

#include "Clients.hpp"
#include "Exceptions.hpp"
#include "Include.hpp"
#include "Parsers.hpp"
#include "Sockets.hpp"

namespace WSN {

class Server {

  private:
	vector<ListeningSocket> listen_socket;
	virtual void			accepter(int accept_socket)	 = 0;
	virtual void			handle(int client_socket)	 = 0;
	virtual void			responder(int client_socket) = 0;

  public:
	// Getters
	ListeningSocket			get_listen_socket(int index) const;
	vector<ListeningSocket> get_listen_socket() const;

	// Constructors and copy constructor and copy assignment operator and destructor
	Server(int domain, int service, int protocol, vector<int> port, u_long interface, int backlog);
	Server(const Server &server);
	Server &operator=(const Server &server);
	virtual ~Server();

	virtual void launch() = 0;
};

} // namespace WSN

#endif // SERVER_HPP