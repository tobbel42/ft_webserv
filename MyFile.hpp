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

// wait
#include <sys/types.h>
#include <sys/wait.h>

#include "get_next_line.h"
#include "MyDirectory.hpp"

class MyFile
{

private:

	std::string _complete_filename;
	std::string _current_url;
	char **_envp;
	bool _directory_listing;



	std::string get_file_content()
	{
		std::ifstream in;
		std::string content;
		std::string line;

		in.open(this->_complete_filename);
		if (in.is_open()) // else??
		{
			while (getline (in,line))
				content = content + line + '\n';
		in.close();
		}
		return (content);
	}

	int	check_file_extension()
	{
		if(this->_complete_filename.substr(this->_complete_filename.find_last_of(".") + 1) == "php")
			return PHP;
		else if(this->_complete_filename.substr(this->_complete_filename.find_last_of(".") + 1) == "py")
			return PYTHON;
		else if (this->_complete_filename.substr(this->_complete_filename.find_last_of(".") + 1) == "html")
			return HTML;
		else if (this->_complete_filename.find_last_of(".") == std::string::npos)
			return FOLDER; // macht das Sinn??
		else
			return HTML; // ist es angreifbar wenn wir sonst einfach die File ausgeben??
	}

	int	create_temp_file(char *filename)
	{
		int fd = mkstemp(filename); // Creates and opens a new temp file r/w.
		if (fd == -1)
			std::cout << "creation of file went wrong" << std::endl; //Errorhandling
		//unlink(filename);
		return fd;
	}

	std::string read_from_file(int fd)
	{
		std::string content;
		char* line = get_next_line(fd);
		while(line != NULL)
		{
			content += line;
			line = get_next_line(fd);
			//printf("line: %s\n", line);
		}
		//std::cout << "content: " << content << std::endl;
		close(fd);
	//   unlink(filename);  
		return content;
	}


	std::string execute_cgi(int file_extension)
	{
		char *argv[3];
		char filename_char[] = "/tmp/mytemp.XXXXXX";
		int fd = create_temp_file(filename_char);

		if (file_extension == PHP)
			argv[0] = "/usr/bin/php";
		else
			argv[0] = "/usr/bin/python";
		argv[1] = (char *) _complete_filename.c_str();
		argv[2] = NULL;

		int pid = fork();
		if (pid == -1)
			std::cout << "Problem bei fork" << std::endl;
		if (pid == 0)
		{
			dup2(fd, STDOUT_FILENO);
			if (execve(argv[0], argv, this->_envp) == -1)
				std::cout << "Problem bei execve" << std::endl; // exiten hier??
		}
		wait(NULL); // Achtung, wenn Endlosschleife, dann kann es hier zu Problemen kommen
		close(fd);
		fsync(fd);
		int fd2 = open(filename_char, 0);
		//unlink(filename_char);
		return (read_from_file(fd2));
	}


	std::string get_directory_content()
	{
		MyDirectory dir(_complete_filename, _current_url);
		return (dir.list_content());
	}


public:
	MyFile(std::string filename, std::string path, std::string current_url, char **envp, bool directory_listing)
		: _complete_filename(path + filename), _current_url(current_url + "/"), _envp(envp), _directory_listing(directory_listing)
	{
	
	}

	std::string read_file()
	{
		//check_valid_filename(folder_path + filename);
		std::cout << "filename: " << this->_complete_filename << std::endl;
		int file_extension = check_file_extension();
		std::cout << "file extension: " << file_extension << std::endl;
		if (file_extension == PHP || file_extension == PYTHON)
			return execute_cgi(file_extension);
		else if (file_extension == HTML)
			return get_file_content();
		else if (file_extension == FOLDER && _directory_listing == true)
			return get_directory_content();
		else
			return ""; // prüfen
	}
};
#endif