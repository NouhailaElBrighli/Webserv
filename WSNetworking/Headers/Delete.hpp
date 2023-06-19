/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/27 16:12:00 by hsaidi            #+#    #+#             */
/*   Updated: 2023/06/14 14:50:26 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DELETE_HPP
#define DELETE_HPP

#include "WSNetworking.hpp"
#include <dirent.h>

class Delete
{
    private:
        MainClient *main_client;
        vector<ConfigLocationParser *> config_location_parser;
    public:
    
        Delete(MainClient *main_client, vector<ConfigLocationParser *>config_location_parser);
        ~Delete();
        void delete_file();
        void empty_file(const std::string& path);
        void del(const std::string &path);
        bool delete_path(const std::string& path);
        bool delete_directory(const std::string& path);
        // void remove_file_or_directory(const std::string& path);
};

#endif