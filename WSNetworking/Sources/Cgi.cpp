/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:38:43 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/18 21:01:11 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "MainClient.hpp"

Cgi::Cgi(MainClient *main_client, vector<ConfigLocationParser *>config_location_parser){
	this->main_client = main_client;
	this->config_location_parser = config_location_parser;
	
}
// Cgi::Cgi(std::string &filename) {
// 	this->filename = filename;
// }
Cgi::~Cgi(){}


//****** i commented this function because i will fixes something tomorrow ****

// void Cgi::readFileContents(std::string &filename) 
// {
//     std::ifstream file(filename.c_str());
//     if (file.is_open()) {
//         std::string line;
//         while (getline(file, line)) {
//             std::cout << line << std::endl;
//         }
//         file.close();
//     } else {
//         std::cout << "Failed to open file: " << filename << std::endl;
//     }
// }

// int Cgi::getFileType(const std::string& filename) {
//     std::size_t dotPos = filename.rfind('.');
//     if (dotPos != std::string::npos) {
//         std::string extension = filename.substr(dotPos + 1);

//         // Convert the extension to lowercase for case-insensitive comparison
//         for (std::size_t i = 0; i < extension.length(); ++i) {
//             extension[i] = std::tolower(extension[i]);
//         }

//         if (extension == "php") {
//             return 1;
//         } else if (extension == "py") {
//             return 0;
//         }
//     }

//     return -1;
// }

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
			// cout << "--------------------------------------------------------\n";
		}
		// getFileType(this->filename);
		set_cgi_env();
	}

}

// setting the cgi_env map
void Cgi::set_cgi_env()
{
	this->cgi_env["REQUEST_METHOD = "] = this->main_client->get_request("Request-Type");
	cout<< "ppppppppppp |" <<this->cgi_env["REQUEST_METHOD ="] << endl;

}

// i just need to put the env in the map and then i will pass it to the execve 
//fork and execve 


