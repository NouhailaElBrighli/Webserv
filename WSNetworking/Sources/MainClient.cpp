#include "MainClient.hpp"
// Getters
const map<string, string> &MainClient::get_request() const { return request_parser->get_request(); }

const string &MainClient::get_request(string key) { return request_parser->get_request(key); }

const bool &MainClient::get_send_receive_status() const { return send_receive_status; }

// Constructors and destructor
MainClient::MainClient() { std::memset(buffer, 0, MAXLINE + 1); }

MainClient::MainClient(int client_socket, ConfigServerParser *config_server_parser)
	: config_server_parser(config_server_parser), request_parser(new RequestParser()), status(200),
	  send_receive_status(true), msg_status(Accurate::OK200().what()),
	  client_socket(client_socket) {
	std::memset(buffer, 0, MAXLINE + 1);

	this->start_handle();
}

MainClient::~MainClient() { delete request_parser; }

// Methods
void MainClient::start_handle() {
	try {
		this->handle(this->client_socket);
		Response Response;
		if (this->request_parser->get_request("Request-Type") == "GET") {
			Response.Get(this);
		}
	} catch (const std::exception &e) {
		print_short_line("catch something");
		this->msg_status = e.what();
		set_header_for_errors_and_redirection();
	}
	send(client_socket, this->header.c_str(), header.size(), 0);
	this->send_receive_status = false;
}

std::string MainClient::Header_reading(int client_socket) {
	int			bytes;
	std::string data;

	while (1) {
		bytes = recv(client_socket, buffer, MAXLINE, 0);
		buffer[bytes] = '\0';
		if (bytes == 0)
			break;
		if (bytes < 0)
			throw Error::BadRequest400();
		data.append(buffer, bytes);
		if (data.find("\r\n\r\n") != string::npos)
			break;
	}
	return (data);
}

std::string &MainClient::Body_reading(int client_socket, std::string &body) {
	int				  n, bytes, count;
	string			  str = this->request_parser->get_request("Content-Length");
	std::stringstream ss(str);
	ss >> n;
	count = body.size();
	while (1 && count != n) {
		bytes = recv(client_socket, buffer, MAXLINE, 0);
		buffer[bytes] = '\0';
		body.append(buffer, bytes);
		count += bytes;
		std::cout << buffer <<std::endl;
		if (bytes < 0)
			throw Error::BadRequest400();
		if (count == n)
			break;
	}
	return (body);
}

void MainClient::handle(int client_socket) {
	string	data;
	string	head;
	string	body;

	//! BODY NEED TO BE FILL IN EXTERNAL FILE
	print_line("Client");
	data = this->Header_reading(client_socket);
	head = data.substr(0, data.find("\r\n\r\n"));
	this->request_parser->run_parse(head);
	// std::cout << *this->request_parser << std::endl;
	if (this->get_request("Transfer-Encoding").size() != 0 && this->get_request("Transfer-Encoding") != "chunked")
		throw Error::NotImplemented501();// transfer encoding exist and different to chunked
	if (this->get_request("Content-Length").size() == 0 && this->get_request("Transfer-Encoding").size() == 0 && this->get_request("Request-Type") == "POST")
		throw Error::BadRequest400();//post without content-length or transfer encoding
	if (this->request_parser->get_request("Request-Type") == "POST" && this->get_request("Content-Length").size() != 0)
	{
		body = data.substr(data.find("\r\n\r\n") + 4);
		this->Body_reading(client_socket, body);
	}
	int location = this->match_location();
	this->set_location(location);
	if (this->config_server_parser->get_config_location_parser()[get_location()]->get_return().size() != 0)
		throw Accurate::MovedPermanently301();
	is_method_allowed_in_location();
	// if (this->config_server_parser->get_config_location_parser()[locate]->get_autoindex() == 0)
	// 	throw Error::Forbidden403();
	// if (body.length() > this->config_server_parser->get_client_max_body_size())
	// 	throw Error::RequestEntityTooLarge413();
	// this->send_receive_status = false;
}

// int MainClient::get_matched_location_for_request_uri() {
// 	// get file name to compare with index
// 	int locate = 0;
// 	for (vector<ConfigLocationParser *>::const_iterator it
// 		 = config_server_parser->get_config_location_parser().begin();
// 		 it != config_server_parser->get_config_location_parser().end(); it++) {
// 		if ((*it)->get_location().find("cgi") != string::npos) {
// 			locate++;
// 			continue;
// 		}
// 		if (this->get_request("Request-URI") == (*it)->get_location()
// 			|| this->get_request("Request-URI") == (*it)->get_root())
// 			return locate;
// 		else if (this->get_request("Request-URI").find((*it)->get_location()) != string::npos)
// 			return locate;
// 		else if (this->get_request("Request-URI").find((*it)->get_root()) != string::npos)
// 			return locate;
// 		locate++;
// 	}
// 	// if ((is_found == false)
// 		throw Error::NotFound404();
// 	return(-1);
// }

void MainClient::is_method_allowed_in_location() {
	for (vector<ConfigLocationParser *>::const_iterator it = config_server_parser->get_config_location_parser().begin();
		 it != config_server_parser->get_config_location_parser().end(); it++) {
		if (this->get_request("Request-URI").find((*it)->get_location()) != string::npos || this->get_request("Request-URI").find((*it)->get_root()) != string::npos) {
			for (size_t i = 0; i < (*it)->get_methods().size(); i++) {
				if ((*it)->get_methods(i) == this->get_request("Request-Type"))
					return;
			}
		}
	}
	throw Error::MethodNotAllowed405();
}

int	MainClient::GetClientSocket()
{
	return (client_socket);
}

int	MainClient::match_location()
{
	std::string str = this->get_request("Request-URI");
	size_t found;
	int locate = 0;
	while (str.size() != 0)
	{
		locate = 0;
		for (vector<ConfigLocationParser *>::const_iterator itr = config_server_parser->get_config_location_parser().begin();
		itr != config_server_parser->get_config_location_parser().end() ; itr++)
		{
			if ((*itr)->get_location() == str)
			{
				std::string new_url = this->get_request("Request-URI");
				new_url.replace(0, str.size() , this->config_server_parser->get_config_location_parser()[locate]->get_root());
				this->request_parser->reset_request_uri(new_url);
				return (locate);
			}
			locate++;
		}
		found = str.find_last_of('/');
		str = str.substr(0, found);
	}
	//!check_if_uri_exist to serve it
	throw Error::NotFound404();
	return (-1);
}

void MainClient::set_header_for_errors_and_redirection()
{
	std::stringstream ss (this->msg_status);
	ss >> this->status;
	if (this->status  < 400) // redirection
	{
		this->header = "HTTP/1.1 ";
		this->header += this->msg_status;
		this->header += "\r\nContent-Length: 0\r\n";
		this->header += "Location: /"; // should use port and host or not ?
		this->header += this->config_server_parser->get_config_location_parser()[get_location()]->get_return();
		this->header += "\r\n\r\n";
		std::cout << "Header of redirection:\n" << this->header << std::endl;
	}
	else // errors
	{
		Response Error;
		Error.SetError(this->msg_status);
		this->header = Error.GetHeader();
		std::cout << "Header of Error:\n" << this->header << std::endl;
	}
}

void	MainClient::set_location(int location)
{
	this->location = location;
}
	
int		MainClient::get_location()
{
	return (location);
}

ConfigServerParser	*MainClient::get_config_server()
{
	return (config_server_parser);
}