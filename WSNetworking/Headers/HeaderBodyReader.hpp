#ifndef HEADERBODYREADER_HPP
#define HEADERBODYREADER_HPP

#include "WSNetworking.hpp"

class HeaderBodyReader {

  private:
	int			client_socket, count, length;
	size_t		size;
	char		buffer[MAXLINE];
	MainClient *main_client;

	std::ofstream outFile;

	string head, head_body, body, body_file_name, chunkSizeStrPart;
	bool   head_status, body_status;

  private:
	// Copy constructor and assignation operator
	HeaderBodyReader();
	HeaderBodyReader(const HeaderBodyReader &);
	HeaderBodyReader &operator=(const HeaderBodyReader &);

  public:
	// Getters
	const string &get_head() const;
	const string &get_body_file_name() const;

	// Setters
	void set_body_file_name(const string & new_name);

	// Constructors and destructor
	HeaderBodyReader(MainClient *main_client);
	~HeaderBodyReader();

	// Methods
  private:
	void throw_still_running();

  public:
	void header_reading();

  private:
	void   check_header_body();
	void   set_new_body_file_name();
	string generate_random_file_name();
	void   open_body_file();
	int	   receive_data(int set);

  public:
	void body_reading();

  private:
	int hex_to_int(string chunkSizeStr);
	int find_chunk_size_in_body_str();
	int find_chunk_size_from_recv();

  public:
	void chunked_body_reading();

  private:
	void chunked_body_from_header();
	void chunked_body();
};

#endif	// HEADERBODYREADER_HPP