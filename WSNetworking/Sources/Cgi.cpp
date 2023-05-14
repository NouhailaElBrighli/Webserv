/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:38:43 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/14 19:14:06 by hsaidi           ###   ########.fr       */
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
    print_line("hello from cgi");
    std::cout << "hello from cgi" << std::endl;
    cout << "ppppppppp\n" << this->main_client->get_request("Request-Type");
    for (vector<ConfigLocationParser *>::const_iterator it
            = this->config_location_parser.begin(); it != this->config_location_parser.end(); it++)
    {   
        if ((*it)->get_location().find("cgi") != string::npos)
        {
            cout << "location : " << (*it)->get_location() << endl;
            cout << "root : " << (*it)->get_root() << endl;
            cout << "cgi_ext_path : " << (*it)->get_cgi_ext_path(".py") << endl;
            cout << "cgi_ext_path : " << (*it)->get_cgi_ext_path(".sh") << endl;
        }   
    }
    
    //throw std::runtime_error(str_red("cgi error"));
}

void Cgi::set_cgi_env()
{
    
}
