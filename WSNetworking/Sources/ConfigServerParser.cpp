#include "ConfigServerParser.hpp"

// Getters
const int &ConfigServerParser::get_port() const {
	return this->port;
}

const int &ConfigServerParser::get_host(int i) const {
	return this->host_v[i];
}

const string &ConfigServerParser::get_host() const {
	return this->host_s;
}

const string &ConfigServerParser::get_server_name() const {
	return this->server_name;
}

const size_t &ConfigServerParser::get_client_max_body_size() const {
	return this->client_max_body_size;
}

const map<int, string> &ConfigServerParser::get_error_page() const {
	return this->error_page;
}

// vector<ConfigLocationParser *> ConfigServerParser::get_config_location_parser() const {
// 	return this->config_location_parser;
// }

// Constructors and copy constructor and copy assignment operator and destructor
ConfigServerParser::ConfigServerParser(string config_server) {
	this->port_status					= false;
	this->host_status					= false;
	this->server_name_status			= false;
	this->client_max_body_size_status	= false;
	this->error_page_status				= false;
	this->config_location_parser_status = false;

	this->parse_config_server(config_server);
}

ConfigServerParser::~ConfigServerParser() {
	// for (size_t i = 0; i < this->config_location_parser.size(); i++) {
	// 	delete this->config_location_parser[i];
	// }
}

// Tools
string ConfigServerParser::red_string(string txt) {
	return C_RED + txt + C_RES;
}

int ConfigServerParser::checkType(string str) {
	int	 i		= 0;
	int	 len	= str.length();
	bool point	= false;
	int	 num	= 0;
	int	 decLen = 0;
	int	 type	= -1; // {1: int} {2: float} {3: double}

	if (str[i] == '-' || str[i] == '+')
		i++;
	if (str[i] == '\0')
		return type;
	while (i < len) {
		if (isdigit(str[i]))
			num++;
		else if (str[i] == '.' || str[i] == ',') {
			if (point || i == len - 1)
				break;
			point = true;
			i++;
			while (i < len) {
				if (isdigit(str[i])) {
					decLen++;
				} else if (str[i] == 'f' && i == len - 1)
					break;
				else
					break;
				i++;
			}
			break;
		} else
			break;
		i++;
	}
	if (decLen > 0) {
		if (str[len - 1] == 'f' && i == len - 1)
			type = 2;
		else if (i == len)
			type = 3;
	} else if (num > 0 && i == len) {
		type = 1;
	}
	return type;
}

int ConfigServerParser::stringToInt(string str) {
	int	 i	  = 0;
	int	 len  = str.length();
	int	 sign = 1;
	long num  = 0;

	if (str[i] == '-') {
		sign = -1;
		i++;
	} else if (str[i] == '+')
		i++;
	if (str[i] == '\0')
		throw std::runtime_error(red_string("Bad Input"));
	while (i < len) {
		if (isdigit(str[i]))
			num = num * 10 + (str[i] - '0');
		else
			throw std::runtime_error(red_string("Bad Input"));
		i++;
	}
	return (static_cast<int>(num * sign));
}

vector<int> ConfigServerParser::split_ip_address(const string &str) {
	vector<int>		  vect_nbr;
	std::stringstream ss_ip_address(str);
	string			  str_nbr;
	int				  type;

	while (std::getline(ss_ip_address, str_nbr, '.')) {
		type = checkType(str_nbr);
		if (type == 1 && str_nbr.length() <= 3)
			try {
				vect_nbr.push_back(stringToInt(str_nbr));
			} catch (...) {
				throw std::runtime_error(red_string("Host Bad Input"));
			}
		else
			throw std::runtime_error(red_string("Host Bad Input"));
	}
	if (vect_nbr.size() != 4)
		throw std::runtime_error(red_string("Host Bad Input"));

	return vect_nbr;
}

vector<int> ConfigServerParser::stringToHost(string host) {
	vector<int> ip_address;

	ip_address = split_ip_address(host);
	if ((ip_address[0] < 1 || 255 < ip_address[0]) || (ip_address[1] < 0 || 255 < ip_address[1]) || (ip_address[2] < 0 || 255 < ip_address[2]) || (ip_address[3] < 0 || 255 < ip_address[3]))
		throw std::runtime_error(red_string("Host Bad Input"));

	return ip_address;
}
// Setters
void ConfigServerParser::set_port(string port) {
	if (this->port_status == true || port.empty() == true) {
		throw std::runtime_error(red_string("Port Error !!"));
	}

	this->port		  = this->stringToInt(port);
	this->port_status = true;
}

// parse if host is in format "127.0.0.1"
void ConfigServerParser::set_host(string host) {
	if (this->host_status == true || host.empty() == true) {
		throw std::runtime_error(red_string("Host Error !!"));
	}

	this->host_v	  = stringToHost(host);
	this->host_s	  = host;
	this->host_status = true;
}

void ConfigServerParser::set_server_name(string server_name) {
	if (this->server_name_status == true || server_name.empty() == true) {
		throw std::runtime_error(red_string("Server Name Error !!"));
	}

	this->server_name		 = server_name;
	this->server_name_status = true;
}

void ConfigServerParser::set_client_max_body_size(string client_max_body_size) {
	if (this->client_max_body_size_status == true || client_max_body_size.empty() == true) {
		throw std::runtime_error(red_string("Client Max Body Size Error !!"));
	}

	this->client_max_body_size		  = stringToInt(client_max_body_size);
	this->client_max_body_size_status = true;
}

void ConfigServerParser::set_error_page(string error_page) {
	int	   status_code;
	string error_page_path;
	if (error_page.empty() == true) {
		throw std::runtime_error(red_string("Error Page Error !!"));
	}

	status_code		= stringToInt(error_page.substr(0, error_page.find(" ")));
	error_page_path = error_page.substr(error_page.find(" ") + 1, error_page.size());
	if (status_code < 400 || 599 < status_code || error_page_path.empty() == true) {
		throw std::runtime_error(red_string("Error Page Error !!"));
	}

	this->error_page[status_code] = error_page_path;
	this->error_page_status		  = true;
}

// void ConfigServerParser::set_config_location_parser(string config_location) {
// 	if (config_location.empty() == true) {
// 		throw std::runtime_error(red_string("Config Location Error !!"));
// 	}
// 	this->config_location_parser.push_back(new ConfigLocationParser(config_location));
// }

// Methods
void ConfigServerParser::parse_config_server(string config_server) {
	size_t pos = 0;

	while ((pos = config_server.find("\n")) != std::string::npos) {
		string line = config_server.substr(0, pos - 1);
		config_server.erase(0, pos + 1);

		if (line.find("listen") != std::string::npos)
			this->set_port(line.substr(line.find(" ") + 1));

		else if (line.find("server_name") != std::string::npos)
			this->set_server_name(line.substr(line.find(" ") + 1));

		else if (line.find("host") != std::string::npos)
			this->set_host(line.substr(line.find(" ") + 1));

		else if (line.find("client_max_body_size") != std::string::npos)
			this->set_client_max_body_size(line.substr(line.find(" ") + 1));

		else if (line.find("error_page") != std::string::npos)
			this->set_error_page(line.substr(line.find(" ") + 1));

		// else if (line.find("location") != std::string::npos)
		// 	this->config_location_parser.push_back(new ConfigLocationParser(line.substr(line.find(" ") + 1)));
	}
	this->check_status();
}

void ConfigServerParser::check_status() {
	if (!this->port_status)
		throw std::runtime_error(red_string("Error: port is missing"));
	if (!this->host_status)
		throw std::runtime_error(red_string("Error: host is missing"));
	if (!this->server_name_status)
		throw std::runtime_error(red_string("Error: server_name is missing"));
	if (!this->client_max_body_size_status)
		throw std::runtime_error(red_string("Error: client_max_body_size is missing"));
	if (!this->error_page_status)
		throw std::runtime_error(red_string("Error: error_page is missing"));
	// if (!this->config_location_parser_status)
	// 	throw std::runtime_error(red_string("Error: config_location_parser is missing"));
}

std::ostream &operator<<(std::ostream &out, const ConfigServerParser &config_server_parser) {
	print_line("Parsed Server");

	out << "port: " << config_server_parser.get_port() << endl;
	out << "host: " << config_server_parser.get_host() << endl;
	out << "server_name: " << config_server_parser.get_server_name() << endl;
	out << "client_max_body_size: " << config_server_parser.get_client_max_body_size() << endl;
	for (std::map<int, string>::const_iterator it = config_server_parser.get_error_page().begin(); it != config_server_parser.get_error_page().end(); ++it) {
		out << "error_page: " << it->first << " " << it->second << endl;
	}
	// for (std::vector<ConfigLocationParser *>::const_iterator it = config_server_parser.get_config_location_parser().begin(); it != config_server_parser.get_config_location_parser().end(); ++it) {
	return out;
}