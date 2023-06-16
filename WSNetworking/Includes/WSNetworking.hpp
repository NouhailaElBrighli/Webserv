#ifndef WSNETWORKING_HPP
#define WSNETWORKING_HPP

// CGI Prototype
class Cgi;

// Client Prototype
class MainClient;

// Exceptions Prototypes
class Accurate;
class Error;

// HeaderBodyReader Prototype
class HeaderBodyReader;

// Parsers Prototypes
class ConfigFileParser;
class ConfigLocationParser;
class ConfigServerParser;
class RequestParser;

// Server Prototype
class MainServer;

// Sockets Prototypes
class Socket;
class BindSocket;
class ListenSocket;

// Response prototypes
class Response;

// Include
#include "Include.hpp"

// CGI
#include "Cgi.hpp"

// Clients
#include "MainClient.hpp"

// Exceptions
#include "Accurate.hpp"
#include "Error.hpp"

// HeaderBodyReader
#include "HeaderBodyReader.hpp"

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

// response
#include "Delete.hpp"
#include "Response.hpp"

#endif	// WSNETWORKING_HPP
