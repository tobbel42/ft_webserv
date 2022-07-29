#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include <cstdio>
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
	CGI(const std::string& filename, const Request & req, char** envp);

	~CGI();

	CGI& operator=(const CGI& other);

	int get_status_code() const { return m_status_code; }
	const std::string& get_content() const { return m_content; }


	std::string run(e_FileType file_type, const std::string& input);

private: // methods
	CGI();

	class FileWrap;

	bool prep_env();
	char ** map_to_env();
	bool prep_files(FileWrap& infile, FileWrap& outfile, const std::string& input);
	bool exec_cgi(FileWrap& infile, FileWrap& outfile, char* argv[]);
	std::string read_output(FileWrap& outfile);
	std::string get_abs_path();

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
	Request m_req;

	std::map<std::string, std::string> m_env;

	char**		p_env;
	int			m_status_code;

};