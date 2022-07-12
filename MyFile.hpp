#ifndef MYFILE_HPP
#define MYFILE_HPP


#define PHP 1
#define PYTHON 2
#define HTML 3
#define FOLDER 4


#include <iostream>
#include <fstream>
#include <string>
// open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>

// wait
#include <sys/types.h>
#include <sys/wait.h>

// #include "get_next_line.hpp"
#include "utils.hpp"
#include "MyDirectory.hpp"


//404 file not found
// 500 internal error
class MyFile
{

private:

	std::string _complete_filename;
	std::string _current_url;
	char **_envp;
	bool _directory_listing;
	int _errorcode;



	std::string get_file_content()
	{
		std::ifstream in;
		std::string content;
		std::string line;

		in.open(this->_complete_filename.c_str());
		if (in.is_open()) // else??
		{
			while (getline (in,line))
				content = content + line + '\n';
		in.close();
		}
		else
			_errorcode = 404;
		return (content);
	}

	int	check_file_extension()
	{
		std::string extension = utils::get_file_ext(_complete_filename);
		if (extension == "php")
			return PHP;
		else if (extension == "py")
			return PYTHON;
		else if (extension == "html")
			return HTML;
		else if (extension.empty())
			return FOLDER; // macht das Sinn??
		else
			return HTML; // ist es angreifbar wenn wir sonst einfach die File ausgeben??
	}

	int	create_temp_file(char *filename)
	{
		int fd = mkstemp(filename); // Creates and opens a new temp file r/w.
		if (fd == -1)
		{
			perror("mkstemp");
			_errorcode = 500;
		}
		return fd;
	}

	std::string read_from_file(FILE* file)
	{
		std::string content;
		#if 1
		char buffer[1000];
		while (fread(buffer, sizeof(*buffer), sizeof(buffer), file) == sizeof(buffer))
			content += buffer;
		content += buffer;
		fclose(file);
		#else
		char* line = get_next_line(fd);
		while(line != NULL)
		{
			content += line;
			line = get_next_line(fd);
		}
		close(fd);
		#endif
		return content;
	}


	std::string execute_cgi(int file_extension)
	{
		char *argv[3];
		char filename_char[] = "/tmp/mytemp.XXXXXX";

		int fd = create_temp_file(filename_char);
		if (file_extension == PHP)
			argv[0] = (char *) "/usr/bin/php";
		else
			argv[0] = (char *) "/usr/bin/python";
		argv[1] = (char *) _complete_filename.c_str();
		argv[2] = NULL;

		pid_t pid = fork();
		if (pid == -1)
		{
			perror("fork");
			_errorcode = 500;
		}
		else if (pid == 0)
		{
			if (dup2(fd, STDOUT_FILENO) == -1)
			{
				perror("dup2");
				std::exit(errno);
			}
			close(fd);
			execve(argv[0], argv, _envp);
			perror("execve");
			std::exit(errno);
		}

		int stat_loc = 0;
		wait(&stat_loc); // Achtung, wenn Endlosschleife, dann kann es hier zu Problemen kommen
		int child_errno = WEXITSTATUS(stat_loc);
		if (child_errno != 0)
			_errorcode = 500;

		close(fd);

		std::string content;
		std::ifstream file(filename_char);
		if (file.is_open())
		{
			content = utils::read_file(file, "\n");
			file.close();
		}
		else
		{
			EPRINT("couldn't open the temp file" << filename_char);
			_errorcode = 500;
		}
		unlink(filename_char); // deletes the temp file after closing
		return (content);
	}


	std::string get_directory_content()
	{
		MyDirectory dir(_complete_filename, _current_url);
		return (dir.list_content());
	}



public:
	MyFile(std::string filename,  std::string current_url, char **envp, bool directory_listing)
		: _complete_filename(filename), _current_url(current_url + "/"), _envp(envp), _directory_listing(directory_listing)
	{
		_errorcode = 200; //current model: ok by default, is set on error
	}

	int	get_error_code() const
	{
		return _errorcode;
	}

	std::string read_file()
	{
		#ifdef VERBOSE
		std::cout << "filename: " << this->_complete_filename << std::endl;
		#endif
		int file_extension = check_file_extension();
		#ifdef VERBOSE
		std::cout << "file extension: " << file_extension << std::endl;
		#endif
		if (file_extension == PHP || file_extension == PYTHON)
			return execute_cgi(file_extension);
		else if (file_extension == HTML)
			return get_file_content();
		else if (file_extension == FOLDER && _directory_listing == true)
		{
			std::string dir_list = get_directory_content();
			if(dir_list == "page not found")
				_errorcode = 500;
			return dir_list;
		}
		else
			return ""; // prüfen
	}
};

#endif