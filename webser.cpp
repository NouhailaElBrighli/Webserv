#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#define PORT 8080
#define BUFFER_SIZE 2048

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cerr << "socket error" << std::endl;
		return (1);
	}
	std::cout << "socket created successfully" << std::endl;
	struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) == -1)
	{
        std::cerr << "bind error" << std::endl;
        return (1);
    }
	std::cout << "socket successfully bound to address" << std::endl;
	 // Listen for incoming connections
    if (listen(sockfd, SOMAXCONN) != 0) {
        std::cerr << "listening error" << std::endl;
        return (1);
    }
    std::cout << "server listening for connections" << std::endl;
	for(;;){
        // Accept incoming connections
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr,
                               (socklen_t *)&host_addrlen);
        if (newsockfd < 0) {
			std::cout << "accept failure" << std::endl;
            continue;
		}
		std::cout << "connection accepted" << std::endl;
		char buff[BUFFER_SIZE];
		int n = read(newsockfd, buff, BUFFER_SIZE);
		buff[n] = '\0';
		if (n < 0)
		{
			std::cout << "ERROR READING" << std::endl;
			exit(0);
		}
		// std::cout << buff << std::endl;
    	char resp[] = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "hello, world\r\n";
		int w = write(newsockfd, resp, strlen(resp));
		if (w < 0)
		{
			std::cout << "ERROR WRITING" << std::endl;
		}
		std::cout << "writing to socket" << std::endl;
        close(newsockfd);
    }
}