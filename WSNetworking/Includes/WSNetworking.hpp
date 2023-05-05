#ifndef WSNETWORKING_HPP
#define WSNETWORKING_HPP

// Clients Prototypes
class MainClient;

// Exceptions Prototypes
class Accurate;
class Error;

// Parsers Prototypes
class ConfigFileParser;
class ConfigServerParser;
class RequestParser;

// Servers Prototypes
class MainServer;

// Sockets Prototypes
class Socket;
class BindingSocket;
class ListeningSocket;

// Include
#include "Include.hpp"

// Clients
#include "MainClient.hpp"

// Exceptions
#include "Accurate.hpp"
#include "Error.hpp"

// Parsers
#include "ConfigFileParser.hpp"
#include "ConfigServerParser.hpp"
#include "RequestParser.hpp"

// Servers
#include "MainServer.hpp"

// Sockets
#include "Socket.hpp"

#include "BindingSocket.hpp"

#include "ListeningSocket.hpp"

#endif // WSNETWORKING_HPP