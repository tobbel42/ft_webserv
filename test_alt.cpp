// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <iostream>
#include <iostream>
#include <fstream>
#include "get_next_line.hpp"

// open
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

// wait
    #include <sys/types.h>
       #include <sys/wait.h>
#define PORT 8080
#define PHP 1
#define PYTHON 2


//#include <sys/event.h>
std::string generateHead(int size)
{
	return ("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:" + std::to_string(size) + "\n\n");
}

bool check_valid_filename(std::string filename) // Achtung, was wollen wir machen, wenn das passiert?
{
	if (filename.find("../") != std::string::npos || filename.find("/../") != std::string::npos)
	{
		std::cout << "This filename is invalid" << std::endl;
		return (false);
	}
	return (true);
}

std::string get_file_content(std::string filename)
{
	// check for valid filename (without .. etc)
	std::ifstream in;
	std::string content;
	std::string line;

	//check_valid_filename(filename); scheint automatisch von Browser schon gemacht zu werden
	
	in.open(filename); // ist das hier wirklich nicht key sensitive?
	//in.open(path);
	std::cout << "Path: " << filename << std::endl;
	if (in.is_open())
	{
		std::cout << "is opened" << std::endl;
		while (getline (in,line))
			content = content + line + '\n';
	in.close();
	}
	return (content);
}

int	check_file_extension(std::string filename)
{
	if(filename.substr(filename.find_last_of(".") + 1) == "php")
		return PHP;
	else if(filename.substr(filename.find_last_of(".") + 1) == "py")
		return PYTHON;
	else
		return 0; // ist es angreifbar wenn wir sonst einfach die File ausgeben??
}

int	create_temp_file(char *filename)
{
	  // template for our file.        
  int fd = mkstemp(filename);    // Creates and opens a new temp file r/w.
      
	                           // Xs are replaced with a unique number.
  printf("filename: %s\n", filename);
  if (fd == -1)
  {
	  std::cout << "creation of file went wrong" << std::endl;
  }       // Check we managed to open the file.
//   write(fd, "abc", 4);           // note 4 bytes total: abc terminating '\0'
//   /* ...
//      do whatever else you want.
//      ... */
//   close(fd);
//   unlink(filename);     
	return fd;
}

std::string read_from_file(int fd)
{
	printf("fd: %i\n", fd);
	std::string content;
	char* line = get_next_line(fd);
	printf("line: %s\n", line);
	while(line != NULL)
	{
		content += line;
		line = get_next_line(fd);
		printf("line: %s\n", line);
	}
	std::cout << "content: " << content << std::endl;
	close(fd);
//   unlink(filename);  
	return content;
}

std::string execute_cgi(std::string filename, int file_extension, char **envp)
{
	char *argv[3];
	if (file_extension == PHP)
		argv[0] = "/usr/bin/php";
	else
		argv[0] = "/usr/bin/python";
	argv[1] = (char *) filename.c_str();
	argv[2] = NULL;
	std::cout << "vor execve"<< std::endl;
	char filename_char[] = "/tmp/mytemp.XXXXXX";
	int fd = create_temp_file(filename_char);
	int pid = fork();
	if (pid == -1)
		std::cout << "Problem bei fork" << std::endl;
	if (pid == 0)
	{
		dup2(fd, STDOUT_FILENO);
		if (execve(argv[0], argv, envp) == -1)
			std::cout << "Problem bei execve" << std::endl; // exiten hier??
	}
	wait(NULL); // Achtung, wenn Endlosschleife, dann kann es hier zu Problemen kommen
	close(fd);
	fsync(fd);
	//sleep(100);
	//unlink(filename_char);
	int fd2 = open(filename_char, 0);
	printf("fd2: %i\n", fd2);
	char buff[20];
	printf("filenamex: %s\n", filename_char);
	// FILE *file = fdopen(fd, "r");
	
	// int ret =read(fd2, buff, 20);
	// buff[20] = '\0';
	// printf("buf file: %s, ret: %i\n", buff, ret);
	return (read_from_file(fd));
}

std::string check_file(std::string filename, std::string folder_path, char **envp)
{
	//check_valid_filename(folder_path + filename);
	std::cout << "filename: " << folder_path + filename << std::endl;
	filename = folder_path + filename;
	int file_extension = check_file_extension(filename);
	std::cout << "file extension: " << file_extension << std::endl;
	if (file_extension == PHP || file_extension == PYTHON)
		return execute_cgi(filename, file_extension, envp);
	else
	return get_file_content(filename);
}

int main(int argc, char const *argv[], char **envp)
{
	int server_fd, new_socket; long valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
		
	// Only this line has been changed. Everything is same.
	char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 19\n\n";
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("cannot create socket"); 
		return 0; 
	}
		
	memset(&address, '\0', sizeof address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
		
	//memset(address.sin_zero, '\0', sizeof address.sin_zero);
		
		
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed"); 
		return 0; 
	}
	if (listen(server_fd, 10) < 0)
	{
		perror("In listen");
		exit(EXIT_FAILURE);
	}

//	int kq;
//struct kevent evSet;

//	kq = kqueue();

//	EV_SET(&evSet, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
//	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
//		perror("kevent");

////struct kevent evSet;
//    struct kevent evList[32];
//    int nev, i;
//    struct sockaddr_storage addr;
//    socklen_t socklen = sizeof(addr);
    //int fd;
	while(1)
	{
		
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		//nev = kevent(kq, NULL, 0, evList, 32, NULL);
        //if (nev < 1)
        //    perror("kevent");
		// for (i=0; i<nev; i++) {
		//	 if (evList[i].flags & EV_EOF) {
        //        printf("disconnect\n");
        //        fd = evList[i].ident;
        //        EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        //        if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
        //            perror("kevent");
        //        //conn_delete(fd);
        //    }
		//	else if (evList[i].ident == server_fd)
		//	{
				if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
				{
					perror("In accept");
					exit(EXIT_FAILURE);
				}
				
				char buffer[30000] = {0};
				valread = read( new_socket , buffer, 30000);
				printf("%s\n",buffer );
				printf("################################\n");
				int i = 0;
				while(buffer[i] != '/')
					i++;
				int j = i;
				while(buffer[j] != ' ')
					j++;
				//printf("############FILENAME\n");
				i++; // wegen /
				//std::cout << "i: " << i << "j: " << j << std::endl;
			
				std::string new_buffer = buffer;
				std::string filename = new_buffer.substr(i, j - i);

				std::string return_req;
				if (filename.length() != 0)
				{


					std::cout << filename << std::endl << std::endl;
					std::ifstream file;
					std::string line;
					std::string all_lines;
					all_lines = check_file(filename, "www/root/", envp);
					//file.open(filename);
					//while (getline(file, line))
					//{
					//	std::cout << line << "\n";
					//	all_lines = all_lines + line;
					//}
					//file.close();
					//while (i < j)
					//{
					//	printf("%c", buffer[i]);
					//	i++;
					//}
					//printf("\n\n");
					std::cout << "all lines: " +  all_lines << std::endl;
					std::string return_req = generateHead(all_lines.length()) + all_lines;
					
					//std::cout << "return req: " << return_req << std::endl;
				write(new_socket , return_req.c_str() , return_req.length());
				}
				else
				{
					return_req.assign(hello, strlen(hello));
				//std::cout << "return req: " << hello << std::endl;
				write(new_socket , hello , strlen(hello));
				}
				printf("------------------Hello message sent-------------------");
				close(new_socket);
			//}
		 //}
	}
	return 0;
}