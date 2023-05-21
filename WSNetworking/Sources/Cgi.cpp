/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:38:43 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/21 20:11:29 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "MainClient.hpp"

Cgi::Cgi(MainClient *main_client, vector<ConfigLocationParser *>config_location_parser){
	this->main_client = main_client;
	this->config_location_parser = config_location_parser;
	
}
Cgi::~Cgi(){}


void Cgi::readFileContents() 
{
	cout<<"*****in readFileContents***\n";
	this->filename = "/Users/hsaidi/Desktop/teamserv/files/file1.php";
    std::ifstream file(filename.c_str());
    if (file.is_open()) {
        std::string line;
	getFileType(filename);
    } else {
        std::cout << "Failed to open file: " << filename << std::endl;
		// i need to send 404 error here 
    }
}


int Cgi::getFileType(const std::string& filename) 
{
	cout<<"*****in getFileType***\n";
    std::size_t dotPos = filename.rfind('.');
    if (dotPos != std::string::npos) {
        std::string extension = filename.substr(dotPos + 1);

        // Convert the extension to lowercase for case-insensitive comparison
        for (std::size_t i = 0; i < extension.length(); ++i) {
            extension[i] = std::tolower(extension[i]);
        }

        if (extension == "php") 
			return 2;
		else if (extension == "py")
			return 1;
		else
			cout << "---- can't accept these extensions ----" << std::endl;
    }
	return -1;
}

// Function to convert a std::map<std::string, std::string> to a char**
char* const* Cgi::mapToCharConstArray(const std::map<std::string, std::string>& cgi_env) {
    char** envp = new char*[cgi_env.size() + 1]; // +1 for the terminating null pointer
    int i = 0;
    
    for (std::map<std::string, std::string>::const_iterator it = cgi_env.begin(); it != cgi_env.end(); ++it) {
        std::string envVar = it->first + "=" + it->second;
        envp[i] = new char[envVar.size() + 1];
        std::strcpy(envp[i], envVar.c_str());
        ++i;
    }
    
    envp[i] = NULL; // Terminating null pointer
    
    return const_cast<char* const*>(reinterpret_cast<char**>(envp));
}

void Cgi::just_print()
{
	cout<<"*****in just_print***\n";
	std::cout << "hello from cgi" << std::endl;
	// cout << "->>>  " << this->main_client->get_request("Request-Type") <<std::endl;
	for(map<string, string>::const_iterator it = this->main_client->get_request().begin(); it != this->main_client->get_request().end(); it++)
	{
		cout << it->first << " : " << it->second << endl;          
	}
	readFileContents();
	for (vector<ConfigLocationParser *>::const_iterator it
			= this->config_location_parser.begin(); it != this->config_location_parser.end(); it++)
	{   
		if ((*it)->get_location().find("cgi") != string::npos)
		{
			if (getFileType(this->filename) == 1)
				this->script = (*it)->get_cgi_ext_path(".py");
			else if (getFileType(this->filename) == 2)
				this->script = (*it)->get_cgi_ext_path(".php");
			else
				cout << "---- can't accept these extensions ----" << std::endl;
			cout << "---------------------------------here------------------------\n";
			// cout << "cgi_ext_path : " << (*it)->get_cgi_ext_path(".py") << endl;
			// cout << "cgi_ext_path : " << (*it)->get_cgi_ext_path(".php") << endl;
		}
	}           
	std::ifstream checl_script(this->script.c_str());
	if (checl_script.is_open())
	{
		set_cgi_env();
	}
	else
		cout << "---- can't open the script ----" << std::endl;
		// i need to send 404 error here
}
// setting the cgi_env map
void Cgi::set_cgi_env()
{
	cout << "---------------------------------------------------------\n";
	cout<<"*****in set_cgi_env***\n";
	cgi_env["REQUEST_METHOD="] = this->main_client->get_request("Request-Type");
	cgi_env["PATH_INFO="] = this->main_client->get_request("Request-URI");
	cgi_env["QUERY_STRING="] = this->main_client->get_request("Query-String");
	cgi_env["HTTP_COOKIE="] = this->main_client->get_request("Cookie");
	cgi_env["SCRIPT_FILENAME="] = this->filename;
	cgi_env["SERVER_PROTOCOL="] = this->main_client->get_request("Protocol-Version");
	cgi_env["GETWAY_INTERFACE="] = "CGI/1.1";
	cgi_env["REDIRECT_STATUS="] = "true";
	cgi_env["REQUEShT_URI="] = this->main_client->get_request("Request-URI");
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

	const char  *av[] = { const_cast<char*>(script.c_str()), this->filename.c_str(), NULL};
	this->env = mapToCharConstArray(cgi_env);
	cout << "-----------------------------------------------------------------------------------\n";
	cout<< "script : " << this->script << endl;
	cout<< "filename : " << this->filename << endl;
	int pid = fork();
	if (pid == 0)
	{
		execve(av[0], av, env);
	}

}
// perce the locationand stor the executable path 
//fork and execve


