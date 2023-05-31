/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:38:43 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/31 17:34:12 by hsaidi           ###   ########.fr       */
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
    std::cout << "*****in readFileContents***\n";
    std::string requestUri = this->main_client->get_request("Request-URI");
    std::string location, file;

    // Check if the Request-URI contains a location or root
    for (std::vector<ConfigLocationParser*>::const_iterator it = this->config_location_parser.begin();
         it != this->config_location_parser.end(); it++) {
        if ((*it)->get_location().length() <= requestUri.length()
            && requestUri.substr(0, (*it)->get_location().length()) == (*it)->get_location()) {
            location = (*it)->get_root(); // Replace location with root
            file = requestUri.substr((*it)->get_location().length());
            break;
        } else if ((*it)->get_root().length() <= requestUri.length()
                   && requestUri.substr(0, (*it)->get_root().length()) == (*it)->get_root()) {
            location = (*it)->get_root();
            file = requestUri.substr((*it)->get_root().length());
            break;
        }
    }

    // If a location or root is found, update the filename
    if (!location.empty()) {
        if (!file.empty() && file[0] == '/')
            file.erase(0, 1); // Remove leading slash if present
        this->filename = location + '/' + file; // Add '/' between location and file
    } else {
        this->filename = requestUri; // Use the original filename
    }

    std::ifstream fileStream(this->filename.c_str());
    if (fileStream.is_open())
        getFileType(this->filename);
    else
        std::cout << "Failed to open file: " << this->filename << std::endl;
}


int Cgi::getFileType(const std::string& filename) 
{
    std::size_t dotPos = filename.rfind('.');
    if (dotPos != std::string::npos) {
        std::string extension = filename.substr(dotPos + 1);

        if (extension == "php") 
			return 2;
		else if (extension == "py")
			return 1;
		else
			cout << "---- can't accept this extension ----" << std::endl;
    }
	return -1;
}

char* const* Cgi::mapToCharConstArray(const std::map<std::string, std::string>& cgi_env) {
    char** envp = new char*[cgi_env.size() + 1];
    int i = 0;
	string envVar;

	for (std::map<std::string, std::string>::const_iterator it = cgi_env.begin(); it != cgi_env.end(); ++it) 
	{
	    envVar = it->first + it->second;
        envp[i] = strdup(envVar.c_str());
        ++i;
    }
    envp[i] = NULL; 
    return const_cast<char* const*>(envp);
}

void Cgi::check_extention()
{
	cout<<"*****in just_print***\n";
	std::cout << "hello from cgi" << std::endl;
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
				cout << "---- can't accept this extension ----" << std::endl;
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
	cgi_env["REQUEST_METHOD="] = this->main_client->get_request("Request-Type");
	cgi_env["PATH_INFO="] = this->main_client->get_request("Request-URI");
	cgi_env["QUERY_STRING="] = this->main_client->get_request("Query-String");
	cgi_env["HTTP_COOKIE="] = this->main_client->get_request("Cookie");
	cgi_env["SCRIPT_FILENAME="] = this->filename;
	cgi_env["SERVER_PROTOCOL="] = this->main_client->get_request("Protocol-Version");
	cgi_env["GATEWAY_INTERFACE="] = "CGI/1.1";
	cgi_env["REDIRECT_STATUS="] = "200";
	cgi_env["REQUEST_URI="] = this->main_client->get_request("Request-URI");
	cgi_env["HTTP_HOST="] = this->main_client->get_request("Host");
	cgi_env["CONTENT_TYPE="] =this->main_client->get_request("Content-Type");
	if (this->main_client->get_request("Request-Type") == "POST")
	{
		cgi_env["CONTENT_LENGTH="] = this->main_client->get_request("Content-Length");
	}
	cout << "------------------- Printing the env variables ------------------------------------\n";
    // std::map<std::string, std::string>::const_iterator it;
    // for (it = cgi_env.begin(); it != cgi_env.end(); ++it) {
    //     const std::string& key = it->first;
    //     const std::string& value = it->second;
    //     std::cout <<"|________|"<< key << value << "|________|"<< std::endl;
    // }

	const char  *av[] = {script.c_str(), this->filename.c_str(), NULL};
	char *const *av2 = const_cast<char *const *>(av);

	this->env = mapToCharConstArray(cgi_env);

	for (size_t i = 0; cgi_env.size() > i; i++)
		cout <<"| "<< this->env[i] <<" |"<< endl;
	cout << "-----------------------------------------------------------------------------------\n";

	output_file = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
	input_file = open("input.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
	int pid = fork();
	if(pid < 0)
	{
		cout << "fork failed" << endl;
		// i need to send 500 error here
	}
	else if (pid == 0)
	{
		dup2(output_file, 1);
		close(output_file);
		dup2(input_file, 0);
		close(input_file);
		execve(av[0], av2, this->env);
	}
	waitpid(pid, NULL, 0);
		// execve(av[0], av2, const_cast<char *const *>(&cgi_env[0]));
}


