#include "HeaderBodyReader.hpp"

// Getters
const string &HeaderBodyReader::get_head() const { return this->head; }

const string &HeaderBodyReader::get_body_file_name() const { return this->body_file_name; }

// Constructors and destructor
HeaderBodyReader::HeaderBodyReader(MainClient *main_client) : main_client(main_client) {
	this->client_socket = main_client->get_client_socket();
	this->count			= 0;
	this->length		= 0;
	this->size			= 0;
	this->head_status	= false;
	this->body_status	= false;
}

HeaderBodyReader::~HeaderBodyReader() {}

// Methods
void HeaderBodyReader::throw_still_running() {
	this->outFile.eof();
	this->outFile.close();
	throw std::runtime_error("Still running");
}

void HeaderBodyReader::header_reading() {
	int bytes;

	if (this->head_status)
		return;

	std::memset(buffer, 0, MAXLINE);
	bytes = recv(this->client_socket, buffer, MAXLINE, 0);
	if (bytes == 0)
		return;
	if (bytes < 0)
		throw Error::BadRequest400();
	this->head.append(buffer, bytes);

	if (this->head.find("\r\n\r\n") != string::npos) {
		this->head_status = true;
		size_t bodyStart  = this->head.find("\r\n\r\n") + 4;
		if (bodyStart < this->head.size()) {
			this->body = this->head.substr(bodyStart);
			this->head = this->head.substr(0, bodyStart);
			return;
		} else {
			this->head_status = true;
			return;
		}
	} else {
		throw std::runtime_error("Still running");
	}
}

void HeaderBodyReader::check_header_body() {
	if (!this->body.empty() && this->body.find("\r\n\r\n") != string::npos) {
		this->head_body = this->body.substr(0, this->body.find("\r\n\r\n") + 4);
		if (this->head_body.find("Content-Disposition") != string::npos) {
			set_new_body_file_name();
		}
	}
}

void HeaderBodyReader::set_new_body_file_name() {
	size_t len	= 10;
	size_t pos1 = this->head_body.find("filename=");
	size_t pos2 = this->head_body.find("\"\r\n");
	if (pos1 == string::npos || pos2 == string::npos)
		return;
	size_t size = pos2 - pos1 - len;

	this->body_file_name = "./tmp/" + this->head_body.substr(pos1 + len, size);
	if (access(this->body_file_name.c_str(), F_OK) == 0) {
		std::stringstream ss;
		ss << "_" << std::hex << std::rand();
		this->body_file_name.insert(this->body_file_name.find_last_of('.'), ss.str());
	}
	SHOW_INFO("new body file name : " + this->body_file_name);
}

string HeaderBodyReader::generate_random_file_name() {
	std::stringstream ss;
	std::time_t		  now		   = std::time(0);
	string			  Content_Type = main_client->get_request("Content-Type");

	// Seed the random number generator
	std::srand(static_cast<unsigned int>(std::time(0)));
	if (Content_Type.empty())
		ss << "./tmp/body_" << std::hex << now << "_" << std::rand() << ".txt";
	else if (main_client->get_mime_type().find(Content_Type) != main_client->get_mime_type().end())
		ss << "./tmp/body_" << std::hex << now << "_" << std::rand()
		   << main_client->get_mime_type(Content_Type);
	else
		ss << "./tmp/body_" << std::hex << now << "_" << std::rand() << ".bin";

	return ss.str();
}

void HeaderBodyReader::open_body_file() {
	if (this->body_file_name.empty()) {
		this->body_file_name = generate_random_file_name();
		SHOW_INFO("body file name : " + this->body_file_name);
		this->check_header_body();
	}

	// Open the file for writing if it's not already open
	if (!this->outFile.is_open()) {
		this->outFile.open(this->body_file_name.c_str(), std::ios::app | std::ios::binary);
		if (!this->outFile)
			throw std::runtime_error(STR_RED("can't open file " + this->body_file_name));
	}
}

int HeaderBodyReader::receive_data(int size) {
	int len = MAXLINE;
	if (size < MAXLINE)
		len = size;

	std::memset(buffer, 0, len);
	int bytes = recv(this->client_socket, buffer, len, 0);
	if (bytes < 0)
		throw Error::BadRequest400();

	// Write data to the file
	this->outFile.write(buffer, bytes);
	this->outFile.flush();

	this->outFile.eof();
	this->outFile.close();	// Close the file

	return bytes;
}

void HeaderBodyReader::body_reading() {
	int bytes;

	if (this->body_status)
		return;

	this->open_body_file();

	if (this->body.size() != 0) {
		this->count = this->body.size();
		this->outFile.write(this->body.c_str(), this->count);
		this->body.clear();
	}

	if (this->length == 0) {
		this->length = ConfigServerParser::stringToInt(main_client->get_request("Content-Length"));
		if (static_cast<size_t>(this->length)
			> this->main_client->get_config_server()->get_client_max_body_size())
			throw Error::RequestEntityTooLarge413();
	}
	if (this->length == 0 || this->length == this->count) {
		this->body_status = true;
		this->count		  = 0;
		return;
	}

	bytes = this->receive_data(MAXLINE);
	this->count += bytes;

	if (this->count == this->length || bytes == 0) {
		this->body_status = true;
		this->count		  = 0;
		return;
	} else {
		throw std::runtime_error("Still running");
	}
}

int HeaderBodyReader::hex_to_int(string chunkSizeStr) {
	int				  i;
	std::stringstream ss(chunkSizeStr);
	ss >> std::hex >> i;
	return i;
}

int HeaderBodyReader::find_chunk_size_in_body_str() {
	int	   len	= 0;
	size_t pos0 = this->body.find("\r\n");
	size_t pos1 = this->body.find("\r\n0\r\n");

	if (pos0 != string::npos && pos0 != 0) {
		string chunkSizeStr = this->body.substr(0, pos0);
		len					= hex_to_int(chunkSizeStr);
	} else if (pos1 != string::npos) {
		len = 0;
	} else {
		this->chunkSizeStrPart = this->body;
		len					   = -1;
	}
	this->body = this->body.substr(pos0 + 2);

	if (len != -1)
		this->size += len;
	if (this->size > this->main_client->get_config_server()->get_client_max_body_size())
		throw Error::RequestEntityTooLarge413();
	return len;
}

int HeaderBodyReader::find_chunk_size_from_recv() {
	int				  bytes, len = 0;
	std::stringstream ss;
	string			  tmp_body;
	bool			  start = true;
	char			  num_buff[2];

	if (this->chunkSizeStrPart.size() != 0) {
		tmp_body.append(this->chunkSizeStrPart);
		this->chunkSizeStrPart.clear();
	}
	for (int i = 0; i < MAXLINE; i++) {
		std::memset(num_buff, 0, 2);
		bytes = recv(this->client_socket, num_buff, 1, 0);
		if (bytes == 0) {
			len = 0;
			break;
		}
		if (bytes < 0)
			throw Error::BadRequest400();

		tmp_body.append(num_buff, bytes);

		if (start && num_buff[0] == '\r')
			continue;
		start = false;

		std::size_t pos = tmp_body.find("\r\n");
		if (pos != string::npos) {
			string chunkSizeStr = tmp_body.substr(0, pos);
			if (chunkSizeStr == "0" || chunkSizeStr.empty()) {
				len = 0;
				break;
			}
			len = hex_to_int(chunkSizeStr);
			break;
		}
	}
	if (len > 0)
		this->size += len;
	if (this->size > this->main_client->get_config_server()->get_client_max_body_size())
		throw Error::RequestEntityTooLarge413();

	return len;
}

void HeaderBodyReader::chunked_body_reading() {
	size_t pos1 = this->body.find("\r\n0\r\n");

	if (this->body_status)
		return;

	this->open_body_file();

	if (this->body.size() > 0 && pos1 != string::npos) {
		this->chunked_body_from_header();
		this->chunked_body_from_header();
	}

	else
		this->chunked_body();
}

void HeaderBodyReader::chunked_body_from_header() {
	this->count = 0;

	if (this->length == 0) {
		this->length = find_chunk_size_in_body_str();

		if (this->length == 0) {
			this->body_status = true;
			this->length	  = 0;
			return;
		} else if (this->length == -1) {
			this->length = 0;
			this->throw_still_running();
		}
	}
	std::size_t pos = this->body.find("\r\n\r\n");
	if (pos != string::npos) {
		string to_write = this->body.substr(0, this->length);
		this->count		= this->length;
		this->outFile.write(to_write.c_str(), this->length);
		this->body.erase(0, this->length);
	} else {
		string to_write = this->body;
		this->count		= to_write.size();
		this->outFile.write(to_write.c_str(), to_write.size());
		this->body.clear();
	}
	this->length -= this->count;
}

void HeaderBodyReader::chunked_body() {
	int bytes;

	if (this->body.size() > 0) {
		chunked_body_from_header();
		this->throw_still_running();
	}

	if (this->length == 0)
		this->length = find_chunk_size_from_recv();

	bytes = this->receive_data(this->length);
	this->length -= bytes;

	if (this->length == 0)
		this->length = find_chunk_size_from_recv();

	if (bytes == 0 && this->length == 0) {
		this->body_status = true;
		this->length	  = 0;
		return;
	} else {
		throw std::runtime_error("Still running");
	}
}
