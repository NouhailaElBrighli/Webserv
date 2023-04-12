#ifndef MAINCLIENT_HPP
#define MAINCLIENT_HPP

#include "Client.hpp"

namespace WSN {

class MainClient : public Client {

  public:
	// Constructors and copy constructor and copy assignment operator and destructor
	MainClient() = default; // Default constructor declaration
	MainClient(int client_socket);
	MainClient(const MainClient &mainClient);
	MainClient &operator=(const MainClient &mainClient);
	virtual ~MainClient();

	virtual void handle(int client_socket);
};

} // namespace WSN

#endif // MAINCLIENT_HPP