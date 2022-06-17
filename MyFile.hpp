#ifndef MYFILE_HPP
#define MYFILE_HPP


#define PHP 1
#define PYTHON 2


#include <iostream>
#include <fstream>
// open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// wait
#include <sys/types.h>
#include <sys/wait.h>
#include "get_next_line.h"

class MyFile
{

private:

	std::string _complete_filename;
	char **_envp;


	// checks for valid filename (without .. etc)
	bool check_valid_filename() // Achtung, was wollen wir machen, wenn das passiert?
	{
		if (this->_complete_filename.find("../") != std::string::npos || this->_complete_filename.find("/../") != std::string::npos)
		{
			std::cout << "This filename is invalid" << std::endl;
			return (false);
		}
		return (true);
	}

	
	std::string get_file_content()
	{
		std::ifstream in;
		std::string content;
		std::string line;

		//check_valid_filename(filename); //scheint automatisch von Browser schon gemacht zu werden
		
		//std::cout << "open file: " << this->_complete_filename << std::endl;
		in.open(this->_complete_filename);
		if (in.is_open()) // else??
		{
			//std::cout << "is opened" << std::endl;
			while (getline (in,line))
				content = content + line + '\n';
		in.close();
		}
		//std::cout << "Content: " << content << std::endl;
		return (content);
	}

	int	check_file_extension()
	{
		if(this->_complete_filename.substr(this->_complete_filename.find_last_of(".") + 1) == "php")
			return PHP;
		else if(this->_complete_filename.substr(this->_complete_filename.find_last_of(".") + 1) == "py")
			return PYTHON;
		else
			return 0; // ist es angreifbar wenn wir sonst einfach die File ausgeben??
	}

	int	create_temp_file(char *filename)
	{
		int fd = mkstemp(filename);    // Creates and opens a new temp file r/w.
		//printf("filename: %s\n", filename);
		if (fd == -1)
		{
			std::cout << "creation of file went wrong" << std::endl;
		}
		//   unlink(filename);??
		return fd;
	}

	std::string read_from_file(int fd)
	{
		//printf("fd: %i\n", fd);
		std::string content;
		char* line = get_next_line(fd);
		//printf("line: %s\n", line);
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
		//std::cout << "vor execve"<< std::endl;

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
		//sleep(100);
		//unlink(filename_char);
		int fd2 = open(filename_char, 0); // wtf warum kann ich das nicht auskommentieren obwohl es nichts macht
		return (read_from_file(fd2));
	}


public:
	MyFile(std::string filename, std::string path, char **envp) : _complete_filename(path + filename), _envp(envp)
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
		else
		return get_file_content();
	}
};
#endif