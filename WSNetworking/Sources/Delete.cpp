/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/27 11:30:30 by hsaidi            #+#    #+#             */
/*   Updated: 2023/06/08 17:15:59 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Delete.hpp"


Delete::Delete(MainClient *main_client, vector<ConfigLocationParser *>config_location_parser)
{
	this->main_client = main_client;
	this->config_location_parser = config_location_parser;
}

Delete::~Delete(){}

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

void Delete::delete_file()
{
    for (std::vector<ConfigLocationParser*>::const_iterator it = this->config_location_parser.begin();
         it != this->config_location_parser.end(); it++) {
        std::string file_name = this->main_client->get_request("Request-URI");

        if ((*it)->get_location().find("cgi") != std::string::npos)
            continue;

        if (this->main_client->get_request("Request-URI").find((*it)->get_location()) != std::string::npos) {
            file_name.erase(0, (*it)->get_location().length());
            file_name = (*it)->get_root() + file_name; // Concatenate root and remaining path segments
            remove_file(file_name);
            break; // Exit the loop after deleting the requested file/folder
        } else if (this->main_client->get_request("Request-URI").find((*it)->get_root()) != std::string::npos) {
            file_name.erase(0, (*it)->get_root().length());
            file_name = (*it)->get_root() + file_name; // Concatenate root and remaining path segments
            remove_file(file_name);
            break; // Exit the loop after deleting the requested file/folder
        }
    }
}

void Delete::remove_file(const std::string &path)
{
	DIR *dir = opendir(path.c_str());
	if (dir != NULL)
	{
		// Directory exists, delete its contents recursively
		struct dirent *entry;
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;

			std::string entry_path = path + "/" + entry->d_name;

			if (entry->d_type == DT_DIR)
			{
				remove_file(entry_path); // Recursively delete subdirectory
			}
			else if (entry->d_type == DT_REG)
			{
				if (remove(entry_path.c_str()) != 0)
					throw Error::No_content204();
				else
					throw Error::Successe200();
			}
		}
		closedir(dir);
	}

	if (remove(path.c_str()) != 0)
		throw Error::No_content204();
	else
		throw Error::Successe200();
}