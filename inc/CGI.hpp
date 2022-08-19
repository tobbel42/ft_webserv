#pragma once

#include <string>
#include <map>

#include <cstdio>
#include <unistd.h>

#include "typedefs.hpp"
#include "Request.hpp"

class CGI
{
public: // methods

	CGI(const CGI& other);
	CGI(const std::string& filename, const Request & req);
	~CGI();

	int											get_status_code() const;
	const std::map<std::string, std::string>&	get_cgi_header() const;
	const std::string&							get_content() const;

	std::string run(e_FileType file_type, const std::string& input,
					const std::string& executable);


private: // methods
	CGI();
	CGI& operator=(const CGI& other);

	class FileWrap;

	bool		prep_env();
	bool		prep_files(FileWrap& infile, FileWrap& outfile, const std::string& input);

	char**		map_to_env();
	void		parse_header(std::string & output);

	bool		exec_cgi(FileWrap& infile, FileWrap& outfile, char* argv[]);
	bool		wait_for_child(pid_t worker_pid);
	std::string	read_output(FileWrap& outfile);


private: // attributes

	std::string							m_filename;
	std::string							m_content;
	const Request&						m_req;
	std::map<std::string, std::string>	m_env;
	std::map<std::string, std::string>	m_cgi_header;
	// the status code will only be used if the executed script didn't provide one
	// or an error occured during preparation
	int									m_status_code;


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
};
