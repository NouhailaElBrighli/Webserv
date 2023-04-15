#ifndef MAINCLIENT_HPP
#define MAINCLIENT_HPP

#include "Client.hpp"

namespace WSN {

class MainClient : public Client {

	MainClient(const MainClient &);
	MainClient &operator=(const MainClient &);

  public:
	// Constructors and copy constructor and copy assignment operator and destructor
	MainClient();
	MainClient(int client_socket);
	virtual ~MainClient();

	virtual void handle(int client_socket);
};

} // namespace WSN

#endif // MAINCLIENT_HPP