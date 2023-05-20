/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:38:43 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/20 11:10:59 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "MainClient.hpp"

Cgi::Cgi(MainClient *main_client, vector<ConfigLocationParser *>config_location_parser){
	this->main_client = main_client;
	this->config_location_parser = config_location_parser;
	
}
Cgi::~Cgi(){}


//****** i commented this function because i will fixes something tomorrow ****

void Cgi::readFileContents() 
{
	this->filename = "/Users/hsaidi/Desktop/teamserv/file1.php";
    std::ifstream file(filename.c_str());
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            std::cout << line << std::endl;
        }
        file.close();
	getFileType(filename);
    } else {
        std::cout << "Failed to open file: " << filename << std::endl;
    }
}

void Cgi::getFileType(const std::string filename) {
    std::size_t dotPos = filename.rfind('.');
    if (dotPos != std::string::npos) {
        std::string extension = filename.substr(dotPos + 1);

        // Convert the extension to lowercase for case-insensitive comparison
        for (std::size_t i = 0; i < extension.length(); ++i) {
            extension[i] = std::tolower(extension[i]);
        }

        if (extension == "ph") {
            cout << "**php**" << endl;
        } else if (extension == "py") {
			cout<< "**py**\n";
    	}
		else
			std::cout << "---- can't accept this extention ----" << filename << std::endl;
			//need to throw an error here
    }
}

void Cgi::just_print()
{
	std::cout << "hello from cgi" << std::endl;
	// cout << "->>>  " << this->main_client->get_request("Request-Type") <<std::endl;
	cout << "---------------------------------------------------------\n";
	for(map<string, string>::const_iterator it = this->main_client->get_request().begin(); it != this->main_client->get_request().end(); it++)
	{
		cout << it->first << " : " << it->second << endl;          
	}
	for (vector<ConfigLocationParser *>::const_iterator it
			= this->config_location_parser.begin(); it != this->config_location_parser.end(); it++)
	{   
		if ((*it)->get_location().find("cgi") != string::npos)
		{
			// cout << "location : " << (*it)->get_location() << endl;
			// cout << "root : " << (*it)->get_root() << endl;
			// cout << "cgi_ext_path : " << (*it)->get_cgi_ext_path(".py") << endl;
			// cout << "cgi_ext_path : " << (*it)->get_cgi_ext_path(".sh") << endl;
		}
		// getFileType(this->filename);
	}
	set_cgi_env();

}

// setting the cgi_env map
void Cgi::set_cgi_env()
{
	std::string script_file ="file1.php";
	cout << "---------------------------------------------------------\n";
	readFileContents();
	cgi_env["REQUEST_METHOD="] = this->main_client->get_request("Request-Type");
	cgi_env["PATH_INFO="] = this->main_client->get_request("Request-URI");
	cgi_env["QUERY_STRING="] = this->main_client->get_request("Query-String");
	cgi_env["HTTP_COOKIE="] = this->main_client->get_request("Cookie");
	cgi_env["SCRIPT_FILENAME="] = this->main_client->get_request("Request-URI");
	cgi_env["SERVER_PROTOCOL="] = this->main_client->get_request("Protocol-Version");
	cgi_env["GETWAY_INTERFACE="] = "CGI/1.1";
	cgi_env["REDIRECT_STATUS="] = "true";
	cgi_env["REQUEST_URI="] = this->main_client->get_request("Request-URI");
	cgi_env["HTTP_HOST="] = this->main_client->get_request("Host");
	cgi_env["SERVER_SOFTWARE="] = "WEBSERVER";
	if (this->main_client->get_request("Request-Type") == "POST")
	{
		cgi_env["CONTENT_LENGTH="] = this->main_client->get_request("Content-Length");
		cgi_env["CONTENT_TYPE="] = this->main_client->get_request("Content-Type");
	}
	cout << "------------------- Printing the env variables -------------------------------------\n";
    std::map<std::string, std::string>::const_iterator it;
    for (it = cgi_env.begin(); it != cgi_env.end(); ++it) {
        const std::string& key = it->first;
        const std::string& value = it->second;
        std::cout << key << value << std::endl;
    }
	cout << "-----------------------------------------------------------------------------------\n";
	
}

//fork and execve 


