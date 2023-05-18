/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:33:49 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/18 18:02:12 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "WSNetworking.hpp"

class Cgi
{
    private:
        MainClient *main_client;
        vector<ConfigLocationParser *> config_location_parser;
        map<string, string> cgi_env;
        // std::string &filename;

    public:
        Cgi(MainClient *main_client, vector<ConfigLocationParser *>config_location_parser);
        // Cgi(std::string &filename);
        ~Cgi();
        void just_print();
        void set_cgi_env();
        // int getFileType(const std::string& filename);
        // void readFileContents(std::string &filename) ;
    // class CgiException : public std::exception
    // {
    //     private:
    //         std::string _msg;
    //     public:
    //         CgiException(std::string msg) : _msg(msg) {}
    //         virtual ~CgiException() throw() {}
    //         virtual const char *what() const throw() { return _msg.c_str(); }
    // };
}; 

// std::string script_name = std::string("SCRIPT_NAME=") + script;
// 	std::string path = std::string("PATH_INFO=") + getValue("URL");
// 	std::string q = std::string ("QUERY_STRING=") + query;
// 	std::string cookie = std::string ("HTTP_COOKIE=") + getValue("Cookie");
// 	std::string method = std::string ("REQUEST_METHOD=") + getValue("Method");
// 	std::string script_filename = std::string ("SCRIPT_FILENAME=") + file_path;
// 	std::string protocol = std::string ("SERVER_PROTOCOL=") + getValue("version");
// 	std::string software = std::string ("SERVER_SOFTWARE=WEBSERV");
// 	std::string redirect = std::string ("REDIRECT_STATUS=true");
// 	std::string uri = std::string("REQUEST_URI=") + getValue("URL");
// 	std::string host = std::string("HTTP_HOST=") + getValue("Host");
// 	std::string port = std::string("SERVER_PORT=") + port_host.first;
// 	std::string gateway = std::string("GATEWAY_INTERFACE=CGI/1.1");
#endif // CGI_HPP