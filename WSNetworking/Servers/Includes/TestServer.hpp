#ifndef TESTSERVER_HPP
#define TESTSERVER_HPP

#include "Server.hpp"

namespace WSN {

class TestServer : public Server {

  private:
	char buffer[1024] = {0};
	int	 new_socket;

	virtual void accepter();
	virtual void handle();
	virtual void respoder();

  public:
	// Constructors and copy constructor and copy assignment operator and destructor
	TestServer(int domain, int service, int protocol, int port, u_long interface, int backlog);
	TestServer(const TestServer &test_server);
	TestServer &operator=(const TestServer &test_server);
	~TestServer();

	virtual void launch();
};
} // namespace WSN

#endif // TESTSERVER_HPP