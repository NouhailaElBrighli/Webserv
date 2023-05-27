/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:33:49 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/27 11:29:06 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "WSNetworking.hpp"

class Cgi
{
    private:
        MainClient *main_client;
        vector<ConfigLocationParser *> config_location_parser;
        map<string, string> cgi_env;

    public:
        Cgi(MainClient *main_client, vector<ConfigLocationParser *>config_location_parser);
        ~Cgi();
        std::string filename;
        std::string script;
        int output_file;
        int input_file;
        void check_extention();
        void set_cgi_env();
        char *const *env;
        void readFileContents() ;
        int getFileType(const std::string& filename);
        char* const* mapToCharConstArray(const std::map<std::string, std::string>& envMap);
}; 

#endif // CGI_HPP