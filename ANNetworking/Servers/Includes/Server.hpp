#ifndef SERVER_HPP
#define SERVER_HPP

#include "Sockets.hpp"

namespace AN {

class Server {
  private:
	ListeningSocket *socket;
	virtual void	 accepter() = 0;
	virtual void	 handle()	= 0;
	virtual void	 respoder() = 0;

  public:
	// Getters
	ListeningSocket *get_socket() const;

	// Constructors and copy constructor and copy assignment operator and destructor
	Server(int domain, int service, int protocol, int port, u_long interface, int backlog);
	Server(const Server &server);
	Server &operator=(const Server &server);
	virtual ~Server();

	virtual void launch() = 0;
};

} // namespace AN

#endif // SERVER_HPP