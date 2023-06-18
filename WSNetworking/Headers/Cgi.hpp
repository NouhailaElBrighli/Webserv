/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nel-brig <nel-brig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:33:49 by hsaidi            #+#    #+#             */
/*   Updated: 2023/06/18 11:05:39 by nel-brig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "WSNetworking.hpp"
#include <sstream>
#include <iomanip>

class Cgi
{
    private:
        MainClient *main_client;
        vector<ConfigLocationParser *> config_location_parser;
        map<string, string> cgi_env;
		std::string outfile;

    public:
        Cgi(MainClient *main_client, vector<ConfigLocationParser *>config_location_parser, std::string filename);
        ~Cgi();
        std::string     filename;
        std::string     script;
        int             output_file;
        int             input_file;
        void            check_extention();
        void            set_cgi_env();
        char            *const *env;
        void            readFileContents();
        void            query_string();
        std::string urlDecode(const std::string& encoded);
        int             getFileType(const std::string& filename);
        char* const*    mapToCharConstArray(const std::map<std::string, std::string>& envMap);
		std::string     get_outfile();
}; 

#endif // CGI_HPP