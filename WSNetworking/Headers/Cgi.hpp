/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:33:49 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/21 20:05:26 by hsaidi           ###   ########.fr       */
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
        void just_print();
        void set_cgi_env();
        std::string filename;
        std::string script;// need to add the path of the script
      char *const *env;
        void readFileContents() ;
       int getFileType(const std::string& filename);
   char* const* mapToCharConstArray(const std::map<std::string, std::string>& envMap);
}; 

#endif // CGI_HPP