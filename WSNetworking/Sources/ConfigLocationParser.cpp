#include "ConfigLocationParser.hpp"

// Getters
const string &ConfigLocationParser::get_location() const {
	return this->location;
}

const bool &ConfigLocationParser::get_autoindex() const {
	return this->autoindex;
}

const string &ConfigLocationParser::get_root() const {
	return this->root;
}

const vector<string> &ConfigLocationParser::get_index() const {
	return this->index;
}

const string &ConfigLocationParser::get_index(int i) const {
	return this->index[i];
}

const string &ConfigLocationParser::get_return() const {
	return this->return_;
}

const vector<string> &ConfigLocationParser::get_methods() const {
	return this->methods;
}

const string &ConfigLocationParser::get_methods(int i) const {
	return this->methods[i];
}

const map<string, string> &ConfigLocationParser::get_cgi_ext_path() const {
	return this->cgi_ext_path;
}

const string &ConfigLocationParser::get_cgi_ext_path(string key) const {
	return this->cgi_ext_path.at(key);
}

// Constructors and copy constructor and copy assignment operator and destructor
ConfigLocationParser::ConfigLocationParser(string config_location) {
	this->location_status	  = false;
	this->root_status		  = false;
	this->index_status		  = false;
	this->return_status		  = false;
	this->methods_status	  = false;
	this->cgi_ext_path_status = false;

	this->parse_config_location(config_location);
}

ConfigLocationParser::~ConfigLocationParser() {
}

// Tools
int ConfigLocationParser::checkType(string str) {
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

int ConfigLocationParser::stringToInt(string str) {
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
		throw std::runtime_error(str_red("Bad Input : " + str));
	while (i < len) {
		if (isdigit(str[i]))
			num = num * 10 + (str[i] - '0');
		else
			throw std::runtime_error(str_red("Bad Input : " + str));
		i++;
	}
	return (static_cast<int>(num * sign));
}

vector<string> ConfigLocationParser::split_methods(const string &str) {
	vector<string>	  vect_mth;
	std::stringstream ss_methods(str);
	string			  str_mth;

	while (std::getline(ss_methods, str_mth, ' ')) {
		if (str_mth.length() <= 6)
			vect_mth.push_back(str_mth);
		else
			throw std::runtime_error(str_red("Allow methods Bad Input : " + str));
	}
	if (vect_mth.size() > 3)
		throw std::runtime_error(str_red("Allow methods Bad Input : " + str));

	return vect_mth;
}

vector<string> ConfigLocationParser::stringToMethods(string host) {
	vector<string> vect_mth;

	vect_mth = this->split_methods(host);
	for (int i = 0; i < vect_mth.size(); i++) {
		if (vect_mth[i] != "GET" && vect_mth[i] != "POST" && vect_mth[i] != "DELETE")
			throw std::runtime_error(str_red("Allow Methods Bad Input : " + host));
	}

	return vect_mth;
}

// Setters
void ConfigLocationParser::set_location(string location) {
	if (this->location_status == true || location.empty())
		throw std::runtime_error(str_red("location Error : " + location));

	this->location		  = location;
	this->location_status = true;
}

void ConfigLocationParser::set_autoindex(string autoindex) {
	if (this->autoindex_status == true || autoindex.empty())
		throw std::runtime_error(str_red("autoindex Error : " + autoindex));

	if (autoindex == "on")
		this->autoindex = true;
	else if (autoindex == "off")
		this->autoindex = false;
	else
		throw std::runtime_error(str_red("autoindex Error : " + autoindex));

	this->autoindex_status = true;
}

void ConfigLocationParser::set_root(string root) {
	if (this->root_status == true || root.empty())
		throw std::runtime_error(str_red("root Error : " + root));

	this->root		  = root;
	this->root_status = true;
}

void ConfigLocationParser::set_index(string index) {
	std::stringstream ss_index(index);
	string			  str_idx;

	if (this->index_status == true || index.empty())
		throw std::runtime_error(str_red("index Error : " + index));

	while (std::getline(ss_index, str_idx, ' '))
		this->index.push_back(str_idx);

	this->index_status = true;
}

void ConfigLocationParser::set_return(string return_) {
	if (this->return_status == true || return_.empty())
		throw std::runtime_error(str_red("return Error : " + return_));

	this->return_		= return_;
	this->return_status = true;
}

void ConfigLocationParser::set_methods(string methods) {
	if (this->methods_status == true || methods.empty())
		throw std::runtime_error(str_red("methods Error : " + methods));

	this->methods		 = this->stringToMethods(methods);
	this->methods_status = true;
}

void ConfigLocationParser::set_cgi_ext_path(string cgi_ext_path) {
	string cgi_ext_path_input = cgi_ext_path;
	string cgi_ext_path_str;
	string error_page_path;

	if (cgi_ext_path.empty() == true) {
		throw std::runtime_error(str_red("Error CGI Ext Path : " + cgi_ext_path_input));
	}

	cgi_ext_path_str = cgi_ext_path.substr(0, cgi_ext_path.find(" "));
	if (cgi_ext_path_str.empty() == true) {
		throw std::runtime_error(str_red("Error CGI Ext Path : " + cgi_ext_path_input));
	}
	cgi_ext_path.erase(0, cgi_ext_path.find(" ") + 1);

	if (cgi_ext_path.find(" ") != string::npos && cgi_ext_path[cgi_ext_path.find(" ") + 1] != '\0') {
		throw std::runtime_error(str_red("Error CGI Ext Path : " + cgi_ext_path_input));
	}

	error_page_path = cgi_ext_path.substr(0, cgi_ext_path.size());
	if (error_page_path.empty() == true) {
		throw std::runtime_error(str_red("Error CGI Ext Path : " + cgi_ext_path_input));
	}

	this->cgi_ext_path[cgi_ext_path_str] = error_page_path;
	this->cgi_ext_path_status			 = true;
}

// Methods
void ConfigLocationParser::parse_config_location(string config_location) {
	size_t pos = 0;
	string line;

	while ((pos = config_location.find("\n")) != string::npos) {
		line = config_location.substr(0, pos - 1);
		config_location.erase(0, pos + 1);

		if (line.find("location") != string::npos)
			this->set_location(line.substr(line.find(" ") + 1, line.find("{")));

		else if (line.find("autoindex") != string::npos)
			this->set_autoindex(line.substr(line.find(" ") + 1));

		else if (line.find("root") != string::npos)
			this->set_root(line.substr(line.find(" ") + 1));

		else if (line.find("return") != string::npos)
			this->set_return(line.substr(line.find(" ") + 1));

		else if (line.find("index") != string::npos)
			this->set_index(line.substr(line.find(" ") + 1));

		else if (line.find("methods") != string::npos)
			this->set_methods(line.substr(line.find(" ") + 1));

		else if (line.find("cgi_ext_path") != string::npos)
			this->set_cgi_ext_path(line.substr(line.find(" ") + 1));

		// else
		// 	throw std::runtime_error(str_red("Error : unknown directive"));
	}
	this->check_status();
}

void ConfigLocationParser::check_status() {
	if (this->cgi_ext_path_status == true) {

		if (this->location_status == false)
			throw std::runtime_error(str_red("Error : location is missing"));
		if (this->root_status == false)
			throw std::runtime_error(str_red("Error : root is missing"));

		if (this->index_status == true)
			throw std::runtime_error(str_red("Error : index is unnecessary"));
		if (this->return_status == true)
			throw std::runtime_error(str_red("Error : return is unnecessary"));
		if (this->methods_status == true)
			throw std::runtime_error(str_red("Error : methods is unnecessary"));
	} else {

		if (this->location_status == false)
			throw std::runtime_error(str_red("Error : location is missing"));
		if (this->root_status == false)
			throw std::runtime_error(str_red("Error : root is missing"));
		if (this->index_status == false)
			throw std::runtime_error(str_red("Error : index is missing"));
		if (this->return_status == false)
			throw std::runtime_error(str_red("Error : return is missing"));
		if (this->methods_status == false)
			throw std::runtime_error(str_red("Error : methods is missing"));
	}

	// if (this->cgi_ext_path_status == false)
	// 	throw std::runtime_error(str_red("Error : cgi_ext_path is missing"));
}

std::ostream &operator<<(std::ostream &os, const ConfigLocationParser &clp) {
	print_line("Parsed Location");

	os << "location : " << clp.get_location() << std::endl;
	os << "autoindex : ";
	if (clp.get_autoindex() == true)
		os << "on" << std::endl;
	else
		os << "off" << std::endl;
	os << "root : " << clp.get_root() << std::endl;
	for (size_t i = 0; i < clp.get_index().size(); i++)
		os << "index : " << clp.get_index()[i] << std::endl;
	if (clp.get_return().empty() == false)
		os << "return : " << clp.get_return() << std::endl;
	for (size_t i = 0; i < clp.get_methods().size(); i++)
		os << "methods : " << clp.get_methods()[i] << std::endl;
	for (std::map<string, string>::const_iterator it = clp.get_cgi_ext_path().begin(); it != clp.get_cgi_ext_path().end(); it++)
		os << "cgi_ext_path : " << it->first << " " << it->second << std::endl;

	return os;
}