/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:38:43 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/14 15:51:12 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "MainClient.hpp"

Cgi::Cgi(MainClient *main_client){
    this->main_client = main_client;
}
Cgi::~Cgi(){}

void Cgi::just_print()
{
    std::cout << "hello from cgi" << std::endl;
    cout << "ppppppppp\n" << this->main_client->get_request("Request-Type");
    // cout << *this->request_parser << endl;
}