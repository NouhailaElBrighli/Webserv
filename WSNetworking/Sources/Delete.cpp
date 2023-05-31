/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/27 11:30:30 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/31 17:34:07 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

Delete::Delete(MainClient *main_client)
{
    this->main_client = main_client;
}

Delete::~Delete(){}

void Delete::delete_file()
{
    string file_name;
    bool   is_found = false;
    for (vector<ConfigLocationParser *>::const_iterator it
         = config_server_parser->get_config_location_parser().begin();
         it != config_server_parser->get_config_location_parser().end(); it++) {
        file_name = this->get_request("Request-URI");

        if ((*it)->get_location().find("cgi") != string::npos)
            continue;
        if (this->get_request("Request-URI").find((*it)->get_location()) != string::npos) {

            file_name.erase(0, (*it)->get_location().length());
            is_found = true;

        } else if (this->get_request("Request-URI").find((*it)->get_root()) != string::npos) {

            file_name.erase(0, (*it)->get_root().length());
            is_found = true;
        }

        if (is_found == true) {
            if (file_name[0] == '/')
                file_name.erase(0, 1);
            if (file_name.length() == 0)
                return;
            for (size_t i = 0; i < (*it)->get_index().size(); i++) {
                if (file_name == (*it)->get_index(i))
                    return;
            }
        }
    }
    if (remove(file_name.c_str()) != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");
}



























// void Cgi::readFileContents() 
// {
//     std::cout << "*****in readFileContents***\n";
//     std::string requestUri = this->main_client->get_request("Request-URI");
//     std::string location, file;

//     // Check if the Request-URI contains a location or root
//     for (std::vector<ConfigLocationParser*>::const_iterator it = this->config_location_parser.begin();
//          it != this->config_location_parser.end(); it++) {
//         if (requestUri.find((*it)->get_location()) != std::string::npos) {
//             location = (*it)->get_root(); // Replace location with root
//             file = requestUri.substr((*it)->get_location().length());
//             break;
//         } else if (requestUri.find((*it)->get_root()) != std::string::npos) {
//             location = (*it)->get_root();
//             file = requestUri.substr(location.length());
//             break;
//         }
//         else if (requestUri.find((*it)->get_root()) != std::string::npos) {
//             location = (*it)->get_root();
//             file = requestUri.substr(location.length());
//             break;
//         }
//     }

//     // If a location or root is found, update the filename
//     if (!location.empty()) {
//         this->filename = location + file;
//     } else {
//         this->filename = requestUri; // Use the original filename
//     }

//     std::ifstream fileStream(this->filename.c_str());
//     if (fileStream.is_open())
//     {
//         std::string line;
//         getFileType(this->filename);
//     }
//     else
//     {
//         std::cout << "Failed to open file: " << this->filename << std::endl;
//         // Send a 404 error response
//         std::cout << "HTTP/1.1 404 Not Found\r\n\r\n";
//         std::cout << "File not found\r\n";
//     }
// }


