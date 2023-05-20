#ifndef WSNETWORKING_HPP
#define WSNETWORKING_HPP

// Clients Prototypes
class MainClient;
class Cgi;

// Exceptions Prototypes
class Accurate;
class Error;

// Parsers Prototypes
class ConfigFileParser;
class ConfigLocationParser;
class ConfigServerParser;
class RequestParser;

// Servers Prototypes
class MainServer;

// Sockets Prototypes
class Socket;
class BindSocket;
class ListenSocket;

// Include
#include "Include.hpp"

// Clients
#include "MainClient.hpp"

// Exceptions
#include "Accurate.hpp"
#include "Error.hpp"

// Parsers
#include "ConfigFileParser.hpp"
#include "ConfigLocationParser.hpp"
#include "ConfigServerParser.hpp"
#include "RequestParser.hpp"

// Servers
#include "MainServer.hpp"

// Sockets
#include "Socket.hpp"

#include "BindSocket.hpp"

#include "ListenSocket.hpp"

#include "Cgi.hpp"

#endif // WSNETWORKING_HPP
