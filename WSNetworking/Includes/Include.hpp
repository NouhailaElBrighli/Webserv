#ifndef INCLUDE_HPP
#define INCLUDE_HPP

// c include with .h
#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// c includes without .h
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

// c++ includes
#include <algorithm>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

enum { READ_PHASE, WRITE_PHASE };

#define MAXLINE 2048

#define C_RES "\033[0m"
#define C_RED "\033[1;31m"
#define C_GREEN "\033[1;32m"
#define C_BLUE "\033[1;34m"
#define C_PURPLE "\033[1;35m"
#define C_CYAN "\033[1;36m"
#define C_YELLOW "\033[1;33m"

#define PRINT_FUNCTION false  // ! TO DEACTIVATE ALL PRINT FUNCTIONS

#ifdef PRINT_FUNCTION

#define PRINT_ERROR(str)                                                                                               \
	cerr << C_RED << "----------------------------------------------------------------" << str << C_RES << endl;
#define PRINT_LONG_LINE(str)                                                                                           \
	cout << C_GREEN << "------------------------------------------------" << str << C_RES << endl;
#define PRINT_LINE(str) cout << C_YELLOW << "--------------------------------" << str << C_RES << endl;
#define PRINT_SHORT_LINE(str) cout << C_CYAN << "----------------" << str << C_RES << endl;
#define SHOW_INFO(str) cout << str << C_RES << endl;

#else

#define PRINT_ERROR(str)
#define PRINT_LONG_LINE(str)
#define PRINT_LINE(str)
#define PRINT_SHORT_LINE(str)
#define SHOW_INFO(str)

#endif

#define STR_RED(str) (std::string(C_RED) + std::string(str) + std::string(C_RES))
#define STR_GREEN(str) (std::string(C_GREEN) + std::string(str) + std::string(C_RES))
#define STR_CYAN(str) (std::string(C_CYAN) + std::string(str) + std::string(C_RES))

typedef struct addrinfo	   t_addrinfo;
typedef struct sockaddr	   t_sockaddr;
typedef struct sockaddr_in t_sockaddr_in;

using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::string;
using std::vector;

#endif	// INCLUDE_HPP