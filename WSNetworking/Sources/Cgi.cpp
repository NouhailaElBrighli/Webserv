/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nel-brig <nel-brig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:38:43 by hsaidi            #+#    #+#             */
/*   Updated: 2023/06/23 18:43:15 by nel-brig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "MainClient.hpp"
#include <signal.h>

Cgi::Cgi(MainClient *main_client, vector<ConfigLocationParser *> config_location_parser, std::string filename) {
	this->main_client			 = main_client;
	this->config_location_parser = config_location_parser;
	this->filename				 = filename;
	this->status = false;
}
Cgi::~Cgi() {
	  for (size_t i = 0; this->cgi_env.size() > i; i++)
    {
        delete[] this->env[i];
    }
    delete[] this->env;
}

void Cgi::readFileContents() {
	std::ifstream fileStream(this->filename.c_str());
	if (fileStream.is_open())
		getFileType(this->filename);
	else
		throw Error::BadRequest400();
}

string Cgi::generate_random_name()
{
	std::stringstream ss;
	std::time_t		  now		   = std::time(0);

	// Seed the random number generator
	ss << "./outfile_" << std::hex << now << "_" << std::rand() << ".txt";
	return ss.str();
}

int Cgi::getFileType(const std::string& filename) 
{
    std::size_t dotPos = filename.rfind('.');
    if (dotPos != std::string::npos) {
        std::string extension = filename.substr(dotPos + 1);

		if (extension == "php")
			this->script = main_client->get_config_server()
							   ->get_config_location_parser()[main_client->get_location()]
							   ->get_cgi_ext_path(".php");
		else if (extension == "py")
			this->script = main_client->get_config_server()
							   ->get_config_location_parser()[main_client->get_location()]
							   ->get_cgi_ext_path(".py");
		else
			throw Error::NotImplemented501();
	}
	std::ifstream checl_script(this->script.c_str());
	if (checl_script.is_open()) {
		set_cgi_env();
	} else {
		cout << "---- can't open the script ----" << std::endl;
		throw Error::NotFound404();
	}
	return -1;
}

char *const *Cgi::mapToCharConstArray(const std::map<std::string, std::string> &cgi_env) {
	char **envp = new char *[cgi_env.size() + 1];
	int	   i	= 0;
	string envVar;

	for (std::map<std::string, std::string>::const_iterator it = cgi_env.begin(); it != cgi_env.end(); ++it) {
		envVar	= it->first + it->second;
		envp[i] = strdup(envVar.c_str());
		++i;
	}
	envp[i] = NULL;
	return const_cast<char *const *>(envp);
}

void Cgi::check_extention() {
	if (status == false)
	{
		cout << "*****in just_print***\n";
		std::cout << "hello from cgi" << std::endl;
		for (map<string, string>::const_iterator it = this->main_client->get_request().begin();
			 it != this->main_client->get_request().end(); it++) {
			cout << it->first << " : " << it->second << endl;
		}
	}
	readFileContents();
}

std::string Cgi::urlDecode(const std::string& encoded) {
  std::ostringstream decoded;

for (std::size_t i = 0; i < encoded.length(); ++i) {
    if (encoded[i] == '%') {
      if (i + 2 < encoded.length() && isxdigit(encoded[i + 1]) && isxdigit(encoded[i + 2])) {
        // Decode the URL-encoded sequence
        std::string hexStr = encoded.substr(i + 1, 2);
        int hexValue;
        std::istringstream(hexStr) >> std::hex >> hexValue;
        decoded << static_cast<char>(hexValue); 
        i += 2;
      } else {
        // Invalid encoding, treat '%' as a regular character
        decoded << encoded[i];
      }
    } else if (encoded[i] == '+') {
      // Replace '+' with space character
      decoded << ' ';
    } else {
      // Copy the character as is
      decoded << encoded[i];
    }
  }

  return decoded.str();
}

void Cgi::query_string() {
  std::string queryString = this->main_client->get_request("Query-String");
  std::string delimiter = "&";
  std::string pairDelimiter = "=";

  size_t pos = 0;
  std::string token;

  	while ((pos = queryString.find(delimiter)) != std::string::npos) {
    token = queryString.substr(0, pos);
    size_t pairPos = token.find(pairDelimiter);
    if (pairPos != std::string::npos) {
      std::string key = token.substr(0, pairPos);
      std::string value = token.substr(pairPos + 1);

      // URL decode the key and value
      key = urlDecode(key);
      value = urlDecode(value);

      // Store key-value pair in cgi_env
      cgi_env[key] = value;
    }

    queryString.erase(0, pos + delimiter.length());
  }

  // Process the last key-value pair
  size_t pairPos = queryString.find(pairDelimiter);
  if (pairPos != std::string::npos) {
    std::string key = queryString.substr(0, pairPos);
    std::string value = queryString.substr(pairPos + 1);

    // URL decode the key and value
    key = urlDecode(key);
    value = urlDecode(value);

    // Store key-value pair in cgi_env
    cgi_env[key] = value;
  }
	// }
}

void Cgi::set_cgi_env()
{
	query_string();
	cgi_env["CONTENT_TYPE="] =this->main_client->get_request("Content-Type");
	cgi_env["CONTENT_LENGTH="] = this->main_client->get_request("Content-Length");
	cgi_env["REQUEST_METHOD="] = this->main_client->get_request("Request-Type");
	cgi_env["PATH_INFO="] = this->filename;
	cgi_env["QUERY_STRING="] = this->main_client->get_request("Query-String");
	cgi_env["HTTP_COOKIE="] = this->main_client->get_request("Cookie");
	cgi_env["SCRIPT_FILENAME="] = this->filename;
	cgi_env["GATEWAY_INTERFACE="] = "CGI/1.1";
	cgi_env["REDIRECT_STATUS="] = "200";
	// cgi_env["SERVER_PORT="] ="8888";
	cgi_env["REQUEST_URI="] = this->main_client->get_request("Request-URI");
	// cgi_env["HTTP_HOST="] = "127.0.0.1";
	cgi_env["HTTP_HOST="] = this->main_client->get_request("Host");
	cout << "------------------- Printing the env variables ------------------------------------\n";
	char *av[] = {(char *)script.c_str(), (char *)this->filename.c_str(), NULL};

	std::cout << "av[0]----------->" << av[0] << std::endl;
	std::cout << "av[1]----------->" << av[1] << std::endl;
	this->env = mapToCharConstArray(cgi_env);
	size_t i;
	for (i = 0; cgi_env.size() > i; i++) {
		cout << "|" << this->env[i] << "|" << endl;
	}
	printf("** %s\n", env[i]);
	cout << "-----------------------------------------------------------------------------------\n";
	outfile = this->generate_random_name();
	output_file = open(outfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
	input_file = open(this->main_client->get_body_file_name().c_str(), O_RDONLY);
	int pid = fork();
	if (pid < 0) {
		cout << "fork failed" << endl;
		return;
	} else if (pid == 0) {
		dup2(output_file, 2);
		dup2(output_file, 1);
		dup2(input_file, 0);
		close(output_file);
		close(input_file);
		if (execve(av[0], av, this->env) < 0)
			exit(1);
	}
	wait_for_child();
	
	// aitpid(pid, NULL, 0);
	// execve(av[0], av2, const_cast<char *const *>(&cgi_env[0]));
}

std::string Cgi::get_outfile() { return (outfile); }

void	Cgi::wait_for_child()
{
	sleep(1);
	while (1)
	{
		test = waitpid(pid, NULL, WNOHANG);
		if (test == 0 && main_client->get_cgi_counter() == 0)
		{
			PRINT_ERROR("first move");
			main_client->set_cgi_status(true);//!
			throw std::runtime_error("Still running");
		}
		else if (test == 0 && main_client->get_cgi_status() == 1)
		{
			PRINT_ERROR("SECOND MOVE");
			// kill(pid, SIGKILL);
			main_client->set_cgi_status(false);
			main_client->set_write_status(true);
			throw Error::LoopDetected508();
		}
		else
		{
			std::cout << "-----------send directly-----------" << std::endl;
			main_client->set_cgi_status(false);//!
			main_client->set_write_status(false);
			break;
		}
	}
}