/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:38:43 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/17 11:41:51 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "MainClient.hpp"

Cgi::Cgi(MainClient *main_client, vector<ConfigLocationParser *>config_location_parser){
	this->main_client = main_client;
	this->config_location_parser = config_location_parser;
}
Cgi::~Cgi(){}

void Cgi::just_print()
{
	std::cout << "hello from cgi" << std::endl;
	// cout << "->>>  " << this->main_client->get_request("Request-Type") <<std::endl;
	cout << "---------------------------------------------------------\n";
	for(map<string, string>::const_iterator it = this->main_client->get_request().begin(); it != this->main_client->get_request().end(); it++)
	{
		cout << it->first << " : " << it->second << endl;          
	}
	// for (vector<ConfigLocationParser *>::const_iterator it
			// = this->config_location_parser.begin(); it != this->config_location_parser.end(); it++)
	// {   
		// if ((*it)->get_location().find("cgi") != string::npos)
		// {
			// cout << "location : " << (*it)->get_location() << endl;
			// cout << "root : " << (*it)->get_root() << endl;
			// cout << "cgi_ext_path : " << (*it)->get_cgi_ext_path(".py") << endl;
			// cout << "cgi_ext_path : " << (*it)->get_cgi_ext_path(".sh") << endl;
			// cout << "--------------------------------------------------------\n";
		// }
	// }
	//throw std::runtime_error(str_red("cgi error"));
}

void Cgi::set_cgi_env()
{
	if(this->main_client->get_request().find("Request-Type") != this->main_client->get_request().end())
	{
		this->cgi_env["REQUEST_METHOD"] = this->main_client->get_request()["Request-Type"];
	}
	setenv("REQUEST_METHOD", this->cgi_env["REQUEST_METHOD"].c_str(), 1);
	setenv("SERVER_PROTOCOL", this->cgi_env["Protocol-Version"].c_str() 1);
	setenv("PATH_INFO", this->cgi_env["Request-URI"].c_str(), 1);
	
}

