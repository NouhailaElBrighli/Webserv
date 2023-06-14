#include "HeaderBodyReader.hpp"

// Getters
const string &HeaderBodyReader::get_head() const { return this->head; }

const string &HeaderBodyReader::get_body_file_name() const { return this->body_file_name; }

// Constructors and destructor
HeaderBodyReader::HeaderBodyReader(MainClient *main_client) : main_client(main_client) {
	this->client_socket = main_client->get_client_socket();
	this->head_status	= false;
	this->body_status	= false;
}

HeaderBodyReader::~HeaderBodyReader() {}

// Methods
void HeaderBodyReader::throw_still_running() {
	outFile.close();
	throw std::runtime_error("Still running");
}

void HeaderBodyReader::header_reading() {
	int bytes;

	if (this->head_status)
		return;

	std::memset(buffer, 0, MAXLINE);
	bytes = recv(this->client_socket, buffer, MAXLINE, 0);	// byte by byte
	if (bytes == 0)
		return;
	if (bytes < 0)
		throw Error::BadRequest400();
	this->head.append(buffer, bytes);
	if (this->head.find("\r\n\r\n") != string::npos) {
		this->body.append(this->head.substr(this->head.find("\r\n\r\n") + 4));
		this->head		  = this->head.substr(0, this->head.find("\r\n\r\n") + 4);
		this->head_status = true;
		return;
	} else
		throw std::runtime_error("Still running");
}

void HeaderBodyReader::check_header_body() {
	if (this->body.find("\r\n\r\n") != string::npos) {
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
	cout << "new body file name : " << this->body_file_name << endl;
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
		cout << "body file name : " << this->body_file_name << endl;
		this->check_header_body();
	}

	// Open the file for writing
	this->outFile.open(this->body_file_name.c_str(), std::ios::app | std::ios::binary);
	if (!this->outFile)
		throw std::runtime_error(str_red("can't open file " + this->body_file_name));
}

int HeaderBodyReader::receive_data(int size) {
	int set = MAXLINE;
	if (size < MAXLINE)
		set = size;

	std::memset(buffer, 0, set);
	int bytes = recv(this->client_socket, buffer, set, 0);
	if (bytes < 0)
		throw Error::BadRequest400();

	// Write data to the file
	this->outFile.write(buffer, bytes);
	this->outFile.flush();

	// this->outFile.eof();
	this->outFile.close();	// Close the file

	return bytes;
}

void HeaderBodyReader::body_reading() {
	static int count = 0;
	int		   n, bytes;

	if (this->body_status)
		return;

	this->open_body_file();

	if (this->body.size() != 0) {
		count = this->body.size();
		outFile.write(this->body.c_str(), count);
		this->body.clear();
	}

	n = ConfigServerParser::stringToInt(main_client->get_request("Content-Length"));
	if (n == 0 || n == count) {
		this->body_status = true;
		count			  = 0;
		return;
	}

	bytes = this->receive_data(MAXLINE);
	count += bytes;

	if (count == n || bytes == 0) {
		this->body_status = true;
		count			  = 0;
		return;
	} else {
		throw std::runtime_error("Still running");
	}
}

int HeaderBodyReader::find_chunk_size_in_body_str() {
	int	   i   = 0;
	size_t pos = this->body.find("\r\n");

	if (pos != string::npos) {
		string			  chunkSizeStr = this->body.substr(0, pos);
		std::stringstream ss(chunkSizeStr);
		ss >> std::hex >> i;
	} else {
		this->chunkSizeStrPart = this->body;
		return 0;
	}
	this->body = this->body.substr(pos + 2);
	return i;
}

int HeaderBodyReader::find_chunk_size_from_recv() {
	int				  bytes;
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
		if (bytes == 0)
			return 0;
		if (bytes < 0)
			throw Error::BadRequest400();

		tmp_body.append(num_buff, bytes);

		if (start && num_buff[0] == '\r')
			continue;
		start = false;

		std::size_t pos = tmp_body.find("\r\n");
		if (pos != std::string::npos) {
			cout << "tmp_body = " << tmp_body << endl;
			std::string chunkSizeStr = tmp_body.substr(0, pos);
			if (chunkSizeStr == "0" || chunkSizeStr.empty())
				return 0;
			std::stringstream ss(chunkSizeStr);
			ss >> std::hex >> i;
			return i;
		}
	}
	return 0;
}

void HeaderBodyReader::chunked_body_reading() {
	static int n = 0;
	int		   bytes, count = 0;

	if (this->body_status)
		return;

	this->open_body_file();
	cout << 1 << endl;
	cout << "this->body.size() = " << this->body.size() << endl;

	if (this->body.size() > 0) {
		if (n == 0) {
			n = find_chunk_size_in_body_str();
			cout << "find_chunk_size_in_body_str() => n = " << n << endl;
			if (n == 0)
				this->throw_still_running();
		}
		std::size_t pos = this->body.find("\r\n\r\n");
		cout << "pos = " << pos << endl;
		cout << "n = " << n << endl;
		if (pos != std::string::npos) {
			string to_write = this->body.substr(0, n);
			cout << "to_write = " << to_write << endl;
			count = to_write.size();
			outFile.write(to_write.c_str(), to_write.size());
			this->body.erase(0, n);
		} else {
			string to_write = this->body;
			cout << "to_write = " << to_write << endl;
			count = to_write.size();
			outFile.write(to_write.c_str(), to_write.size());
			this->body.clear();
		}
		n -= count;

		this->throw_still_running();
	}

	cout << 2 << endl;
	cout << "n = " << n << endl;
	if (n == 0) {
		n = find_chunk_size_from_recv();
		cout << "find_chunk_size_from_recv() => n = " << n << endl;
	}
	cout << 3 << endl;
	bytes = this->receive_data(n);
	n -= bytes;
	cout << 4 << endl;
	if (n == 0) {
		n = find_chunk_size_from_recv();
		cout << "find_chunk_size_from_recv() => n = " << n << endl;
	}
	cout << 5 << endl;
	if (bytes == 0 && n == 0) {
		cout << "done" << endl;
		this->body_status = true;
		n				  = 0;
		return;
	} else {
		throw std::runtime_error("Still running");
	}
}
