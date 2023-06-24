/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nel-brig <nel-brig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 11:33:49 by hsaidi            #+#    #+#             */
/*   Updated: 2023/06/23 22:00:13 by nel-brig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "WSNetworking.hpp"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdexcept>
# include <sys/time.h>

class Cgi
{
    private:
        MainClient *main_client;
        vector<ConfigLocationParser *> config_location_parser;
        map<string, string> cgi_env;
		std::string outfile;
		int	status;
		int test;
		int pid;
		size_t	_time;
		size_t _phase;
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
		string			generate_random_name();
        int             getFileType(const std::string& filename);
        char* const*    mapToCharConstArray(const std::map<std::string, std::string>& envMap);
		std::string     get_outfile();
		void			wait_for_child();



	size_t	get_time(void)
	{
		struct timeval	t;

		gettimeofday(&t, NULL);
		return (t.tv_sec * 1000 + t.tv_usec / 1000);
	}

}; 

#endif // CGI_HPP