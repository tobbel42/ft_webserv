#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include <cstdio>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>
#include <map>
#include <vector>
#include <cstdlib>

#include "typedefs.hpp"
#include "utils.hpp"
#include "Request.hpp"

class CGI
{
public: // methods
	CGI(const CGI& other);
	CGI(const std::string& filename, const Request & req);

	~CGI();

	CGI& operator=(const CGI& other);

	int get_status_code() const { return m_status_code; }
	const std::map<std::string, std::string> & get_cgi_header();
	const std::string& get_content() const { return m_content; }


	std::string run(e_FileType file_type, const std::string& input,
					const std::string& executable);

private: // methods
	CGI();

	class FileWrap;

	bool prep_env();
	char ** map_to_env();
	bool prep_files(FileWrap& infile, FileWrap& outfile, const std::string& input);
	bool exec_cgi(FileWrap& infile, FileWrap& outfile, char* argv[]);
	bool wait_for_child(pid_t worker_pid);
	std::string read_output(FileWrap& outfile);
	void parse_header(std::string & output);

private: // subclass
	/*
	A wrapper for FILE* objects similar to std::unique_ptr in order to avoid
	manually closing the file
	*/
	class FileWrap
	{
	public:
		FileWrap();
		FileWrap(FILE* file);

		~FileWrap();


		operator FILE* () const;

		void set_file(FILE* file);

		bool open(const std::string& filename, const char* mode);
		bool open_tmpfile();
		void close();


	private:
		FileWrap(const FileWrap&);
		FileWrap& operator=(const FileWrap&);

		FILE* p_file;
	};

private: // attributes
	std::string	m_filename;
	std::string	m_content;
	const Request& m_req;

	std::map<std::string, std::string> m_env;
	std::map<std::string, std::string> m_cgi_header;
	int			m_status_code;

};