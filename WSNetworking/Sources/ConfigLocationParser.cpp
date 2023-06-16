#include "ConfigLocationParser.hpp"

// Getters
const string &ConfigLocationParser::get_location() const { return this->location; }

const bool &ConfigLocationParser::get_autoindex() const {
	if (this->autoindex_status == false)
		return this->autoindex_status;
	return this->autoindex;
}

const string &ConfigLocationParser::get_root() const { return this->root; }

const vector<string> &ConfigLocationParser::get_index() const {
	if (this->index_status == false) {
		// return an empty vector if index is not set
		static vector<string> empty_vector;
		return empty_vector;
	}
	return this->index;
}

const string &ConfigLocationParser::get_index(int i) const {
	if (this->index_status == false) {
		// return an empty string if index is not set
		static string empty_string;
		return empty_string;
	}
	return this->index[i];
}

const string &ConfigLocationParser::get_return() const {
	if (this->return_status == false) {
		// return an empty string if return is not set
		static string empty_string;
		return empty_string;
	}
	return this->return_;
}


const string &ConfigLocationParser::get_upload() const {
	if (this->upload_status == false) {
		// upload an empty string if upload is not set
		static string empty_string;
		return empty_string;
	}
	return this->upload;
}

const vector<string> &ConfigLocationParser::get_methods() const {
	if (this->methods_status == false) {
		// return an empty vector if methods is not set
		static vector<string> empty_vector;
		return empty_vector;
	}
	return this->methods;
}

const string &ConfigLocationParser::get_methods(int i) const {
	if (this->methods_status == false) {
		// return an empty string if methods is not set
		static string empty_string;
		return empty_string;
	}
	return this->methods[i];
}

const map<string, string> &ConfigLocationParser::get_cgi_ext_path() const {
	if (this->cgi_ext_path_status == false) {
		// return an empty map if cgi_ext_path is not set
		static map<string, string> empty_map;
		return empty_map;
	}
	return this->cgi_ext_path;
}

const string &ConfigLocationParser::get_cgi_ext_path(string key) const {
	if (this->cgi_ext_path_status == false) {
		// return an empty string if cgi_ext_path is not set
		static string empty_string;
		return empty_string;
	}
	return this->cgi_ext_path.at(key);
}

// Constructors and copy constructor and copy assignment operator and destructor
ConfigLocationParser::ConfigLocationParser(string config_location)
	: config_location(config_location) {
	this->location_status	  = false;
	this->root_status		  = false;
	this->autoindex_status	  = false;
	this->index_status		  = false;
	this->return_status		  = false;
	this->upload_status		  = false;
	this->methods_status	  = false;
	this->cgi_ext_path_status = false;

	this->parse_config_location();
}

ConfigLocationParser::~ConfigLocationParser() {}

// Tools
bool ConfigLocationParser::check_file(string name, string input, string file_path) {
	struct stat file_info;

	if (stat(file_path.c_str(), &file_info) != 0)
		// Failed to stat file
		throw std::runtime_error(
			STR_RED(name + " Error : " + input + " => '" + file_path + "' does not exist"));

	if (S_ISDIR(file_info.st_mode))
		// File is a directory
		throw std::runtime_error(
			STR_RED(name + " Error : " + input + " => '" + file_path + "' is a directory"));

	if (S_ISREG(file_info.st_mode))
		// File is a regular file
		return true;

	// File is not a directory or a regular file
	throw std::runtime_error(STR_RED(name + " Error : " + input + " => '" + file_path
									 + "' is not a directory or a regular file"));
	return false;
}

vector<string> ConfigLocationParser::split_methods(const string &str) {
	vector<string>	  vect_mth;
	std::stringstream ss_methods(str);
	string			  str_mth;

	while (std::getline(ss_methods, str_mth, ' ')) {
		if (str_mth.length() <= 6)
			vect_mth.push_back(str_mth);
		else
			throw std::runtime_error(STR_RED("Allow methods Bad Input : " + str));
	}
	if (vect_mth.size() > 3)
		throw std::runtime_error(STR_RED("Allow methods Bad Input : " + str));

	return vect_mth;
}

vector<string> ConfigLocationParser::stringToMethods(string host) {
	vector<string> vect_mth;

	vect_mth = this->split_methods(host);
	for (size_t i = 0; i < vect_mth.size(); i++) {
		if (vect_mth[i] != "GET" && vect_mth[i] != "POST" && vect_mth[i] != "DELETE")
			throw std::runtime_error(STR_RED("Allow Methods Bad Input : " + host));
	}

	for (size_t i = 0; i < vect_mth.size(); i++) {
		for (size_t j = i + 1; j < vect_mth.size(); j++) {
			if (vect_mth[i] == vect_mth[j])
				throw std::runtime_error(STR_RED("Allow Methods Bad Input : " + host));
		}
	}

	return vect_mth;
}

bool ConfigLocationParser::find_compare(string &line, const string &str) {
	size_t pos = line.find(str);

	if (pos != string::npos && pos == 0 && str.length() == line.find(" "))
		return true;
	return false;
}

// Setters
void ConfigLocationParser::set_location(string location, size_t pos) {
	location = location.substr(0, location.size() - 2);
	if (this->location_status == true || location.empty() || this->config_location[pos - 1] != '{'
		|| this->config_location[pos - 2] != ' ' || location[0] != '/')
		throw std::runtime_error(STR_RED("location Error : " + location));

	this->location		  = location;
	this->location_status = true;
}

void ConfigLocationParser::set_autoindex(string autoindex, size_t pos) {
	autoindex = autoindex.substr(0, autoindex.size() - 1);

	if (this->autoindex_status == true || autoindex.empty() || this->config_location[pos - 1] != ';'
		|| !std::isalpha(this->config_location[pos - 2]))
		throw std::runtime_error(STR_RED("autoindex Error : " + autoindex));

	if (autoindex == "on")
		this->autoindex = true;
	else if (autoindex == "off")
		this->autoindex = false;
	else
		throw std::runtime_error(STR_RED("autoindex Error : " + autoindex));

	this->autoindex_status = true;
}

void ConfigLocationParser::set_root(string root, size_t pos) {
	root = root.substr(0, root.size() - 1);
	if (this->root_status == true || root.empty() || this->config_location[pos - 1] != ';'
		|| root[0] != '/' || (root.length() > 2 && !std::isalnum(this->config_location[pos - 2])))
		throw std::runtime_error(STR_RED("root Error : " + root));

	struct stat dir_info;

	if (stat(root.c_str(), &dir_info) != 0)
		// Failed to stat directory
		throw std::runtime_error(STR_RED("root Error => '" + root + "' does not exist"));

	if (S_ISREG(dir_info.st_mode))
		// File is a regular file
		throw std::runtime_error(STR_RED("root Error => '" + root + "' is a regular file"));

	if (S_ISDIR(dir_info.st_mode)) {
		// File is a directory
		this->root		  = root;
		this->root_status = true;
		return;
	}

	// File is not a directory or a regular file
	throw std::runtime_error(
		STR_RED("root Error => '" + root + "' is not a directory or a regular file"));
}

void ConfigLocationParser::set_index(string index, size_t pos) {
	index = index.substr(0, index.size() - 1);
	std::stringstream ss_index(index);
	string			  str_idx;

	if (this->index_status == true || index.empty() || this->config_location[pos - 1] != ';'
		|| !std::isalnum(this->config_location[pos - 2]))
		throw std::runtime_error(STR_RED("index Error : " + index));

	while (std::getline(ss_index, str_idx, ' '))
		this->index.push_back(str_idx);

	this->index_status = true;
}

void ConfigLocationParser::set_return(string return_, size_t pos) {
	string return_save = return_;
	return_			   = return_.substr(0, return_.size() - 1);
	if (this->return_status == true || return_.empty() || this->config_location[pos - 1] != ';'
		|| !std::isalnum(this->config_location[pos - 2]))
		throw std::runtime_error(STR_RED("return Error : " + return_));

	this->return_		= return_;
	this->return_status = true;
}

void ConfigLocationParser::set_upload(string upload, size_t pos) {
	string upload_save = upload;
	upload			   = upload.substr(0, upload.size() - 1);
	if (this->upload_status == true || upload.empty() || this->config_location[pos - 1] != ';'
		|| !std::isalnum(this->config_location[pos - 2]))
		throw std::runtime_error(STR_RED("upload Error : " + upload));

	if (upload_save[0] != '/')
		throw std::runtime_error(STR_RED("upload Error : " + upload));
	for (size_t i = 1; i < upload.length(); i++) {
		if (!std::isalnum(upload[i]) && upload[i] != '_') {
			throw std::runtime_error(STR_RED("upload Error : " + upload));
		}
	}

	this->upload		= upload;
	this->upload_status = true;
}

void ConfigLocationParser::set_methods(string methods, size_t pos) {
	methods = methods.substr(0, methods.size() - 1);
	if (this->methods_status == true || methods.empty() || this->config_location[pos - 1] != ';'
		|| !std::isalnum(this->config_location[pos - 2]))
		throw std::runtime_error(STR_RED("methods Error : " + methods));

	this->methods		 = this->stringToMethods(methods);
	this->methods_status = true;
}

void ConfigLocationParser::set_cgi_ext_path(string cgi_ext_path, size_t pos) {
	string cgi_ext_path_input = cgi_ext_path;
	string cgi_ext, cgi_path;

	if (cgi_ext_path.empty() == true) {
		throw std::runtime_error(STR_RED("Error CGI Ext Path : " + cgi_ext_path_input));
	}

	cgi_ext = cgi_ext_path.substr(0, cgi_ext_path.find(" "));
	if (cgi_ext.empty() == true || cgi_ext.length() == 1 || cgi_ext[0] != '.') {
		throw std::runtime_error(STR_RED("Error CGI Ext Path : " + cgi_ext_path_input));
	}
	// check if cgi_ext is contain only characters
	for (size_t i = 1; i < cgi_ext.length(); i++) {
		if (!std::isalpha(cgi_ext[i]))
			throw std::runtime_error(STR_RED("Error CGI Ext Path : " + cgi_ext_path_input));
	}
	cgi_ext_path.erase(0, cgi_ext_path.find(" ") + 1);

	if (cgi_ext_path.find(" ") != string::npos
		&& cgi_ext_path[cgi_ext_path.find(" ") + 1] != '\0') {
		throw std::runtime_error(STR_RED("Error CGI Ext Path : " + cgi_ext_path_input));
	}

	cgi_path = cgi_ext_path.substr(0, cgi_ext_path.size() - 1);
	if (cgi_path.empty() == true || this->config_location[pos - 1] != ';' || cgi_path[0] != '/'
		|| !std::isalnum(this->config_location[pos - 2])) {
		throw std::runtime_error(STR_RED("Error CGI Ext Path : " + cgi_ext_path_input));
	}

	if (this->check_file("CGI Ext Path", cgi_ext_path_input, cgi_path)) {
		this->cgi_ext_path[cgi_ext] = cgi_path;
		this->cgi_ext_path_status	= true;
	}
}

// Methods
void ConfigLocationParser::parse_config_location() {
	size_t pos = 0;
	string line;

	while ((pos = config_location.find("\n")) != string::npos) {
		line = config_location.substr(0, pos);

		if (this->find_compare(line, "location"))
			this->set_location(line.substr(line.find(" ") + 1, line.find("{")), pos);

		else if (this->find_compare(line, "autoindex"))
			this->set_autoindex(line.substr(line.find(" ") + 1), pos);

		else if (this->find_compare(line, "root"))
			this->set_root(line.substr(line.find(" ") + 1), pos);

		else if (this->find_compare(line, "return"))
			this->set_return(line.substr(line.find(" ") + 1), pos);

		else if (this->find_compare(line, "upload"))
			this->set_upload(line.substr(line.find(" ") + 1), pos);

		else if (this->find_compare(line, "index"))
			this->set_index(line.substr(line.find(" ") + 1), pos);

		else if (this->find_compare(line, "methods"))
			this->set_methods(line.substr(line.find(" ") + 1), pos);

		else if (this->find_compare(line, "cgi_ext_path"))
			this->set_cgi_ext_path(line.substr(line.find(" ") + 1), pos);

		else
			throw std::runtime_error(STR_RED("Error : unknown directive '" + line + "'"));

		config_location.erase(0, pos + 1);
	}
	this->check_status();
}

void ConfigLocationParser::check_status() {

	if (!this->location_status)
		throw std::runtime_error(STR_RED("Error : location is missing"));
	if (!this->root_status)
		throw std::runtime_error(STR_RED("Error : root is missing"));
	if (!this->methods_status)
		throw std::runtime_error(STR_RED("Error : methods is missing"));
}

std::ostream &operator<<(std::ostream &os, const ConfigLocationParser &clp) {
	PRINT_LINE("Parsed Location");

	// os << "location : " << clp.get_location() << std::endl;
	// os << "autoindex : ";
	if (clp.get_autoindex() == true)
		os << "on" << std::endl;
	else
		os << "off" << std::endl;
	os << "root : " << clp.get_root() << std::endl;
	for (size_t i = 0; i < clp.get_index().size(); i++)
		os << "index : " << clp.get_index()[i] << std::endl;
	if (!clp.get_return().empty())
		os << "return : " << clp.get_return() << std::endl;
	if (!clp.get_upload().empty())
		os << "upload : " << clp.get_upload() << std::endl;
	for (size_t i = 0; i < clp.get_methods().size(); i++)
		os << "methods : " << clp.get_methods()[i] << std::endl;
	for (std::map<string, string>::const_iterator it = clp.get_cgi_ext_path().begin();
		 it != clp.get_cgi_ext_path().end(); it++)
		os << "cgi_ext_path : " << it->first << " " << it->second << std::endl;

	return os;
}