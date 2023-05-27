/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsaidi <hsaidi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/27 11:30:30 by hsaidi            #+#    #+#             */
/*   Updated: 2023/05/27 18:44:28 by hsaidi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Map to store server resources
// std::map<std::string, std::string> resources;

// // Function to handle DELETE requests
// void handleDeleteRequest(const std::string& resourcePath) {
//     // Check if the resource exists
//     if (resources.find(resourcePath) != resources.end()) {
//         // Delete the resource
//         resources.erase(resourcePath);

//         // Set the appropriate response
//         std::cout << "HTTP/1.1 204 No Content\r\n\r\n";
//     } else {
//         // Resource not found
//         std::cout << "HTTP/1.1 404 Not Found\r\n\r\n";
//         std::cout << "Resource not found\r\n";
//     }
// }

// // Function to parse the HTTP request and handle it accordingly
// void processRequest(const std::string& request) {
//     // Parse the request to extract the method and resource path
//     std::string method;
//     std::string resourcePath;

//     size_t methodEndPos = request.find(' ');
//     if (methodEndPos != std::string::npos) {
//         method = request.substr(0, methodEndPos);

//         size_t pathStartPos = methodEndPos + 1;
//         size_t pathEndPos = request.find(' ', pathStartPos);
//         if (pathEndPos != std::string::npos) {
//             resourcePath = request.substr(pathStartPos, pathEndPos - pathStartPos);

//             // Handle the request based on the method
//             if (method == "DELETE") {
//                 handleDeleteRequest(resourcePath);
//             } else {
//                 // Unsupported method
//                 std::cout << "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
//                 std::cout << "Unsupported method\r\n";
//             }
//         }
//     }
// }

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
