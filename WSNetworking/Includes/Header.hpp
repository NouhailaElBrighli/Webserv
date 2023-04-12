#ifndef HEADER_HPP
#define HEADER_HPP

#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
// inet_ntop
#include <arpa/inet.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <exception>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#define MAXLINE 4096

#define C_RES "\033[0m"
#define C_RED "\033[1;31m"
#define C_GREEN "\033[1;32m"
#define C_CYAN "\033[1;36m"
#define C_YELLOW "\033[1;33m"

typedef struct sockaddr_in t_sockaddr_in;
typedef struct sockaddr	   t_sockaddr;

using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::string;
using std::vector;

// print the line with the step where we are
void print_line(string str) {
	cout << C_CYAN << "-------------------------------------------- " << str << C_RES << endl;
}

#endif // HEADER_HPP
