/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/27 11:30:30 by hsaidi            #+#    #+#             */
/*   Updated: 2023/06/04 18:41:40 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Delete.hpp"

Delete::Delete(MainClient *main_client, vector<ConfigLocationParser *>config_location_parser)
{
	this->main_client = main_client;
	this->config_location_parser = config_location_parser;
}

Delete::~Delete(){}

void Delete::delete_file()
{
    bool is_found = false;

    for (std::vector<ConfigLocationParser*>::const_iterator it = this->config_location_parser.begin();
         it != this->config_location_parser.end(); it++) {
        std::string file_name = this->main_client->get_request("Request-URI");

        if ((*it)->get_location().find("cgi") != std::string::npos)
            continue;

        if (this->main_client->get_request("Request-URI").find((*it)->get_location()) != std::string::npos) {
            cout << "++ location ++" << endl;
            file_name.erase(0, (*it)->get_location().length());
            file_name = (*it)->get_root() + file_name; // Concatenate root and remaining path segments
            is_found = true;
        } else if (this->main_client->get_request("Request-URI").find((*it)->get_root()) != std::string::npos) {
            cout << "++ in delete file root ++" << endl;
            file_name.erase(0, (*it)->get_root().length());
            file_name = (*it)->get_root() + file_name; // Concatenate root and remaining path segments
            is_found = true;
        }

        if (is_found) {
            if (file_name[0] == '/')
                file_name.erase(0, 1);

            if (file_name.empty())
                return;

            for (size_t i = 0; i < (*it)->get_index().size(); i++) {
                if (file_name == (*it)->get_index(i))
                    return;
            }

            if (remove(file_name.c_str()) != 0) {
                cout << "file error" << endl;
                perror("Error deleting file");
                cout << "filename = " << file_name << endl;
                cout << "errno: " << errno << endl;
            } else {
                cout << "file deleted" << endl;
                puts("File successfully deleted");
            }
        }
    }
}
