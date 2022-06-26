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
#include "MyFile.hpp"

// open
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

// wait
    #include <sys/types.h>
       #include <sys/wait.h>
#define PORT 8080



//#include <sys/event.h>
std::string generateHead(int size)
{
	return ("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:" + std::to_string(size) + "\n\n");
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

				//std::string return_req;
				//if (filename.length() != 0)
				//{


					std::cout << filename << std::endl << std::endl;
					std::ifstream file;
					std::string line;
					std::string all_lines;
					MyFile myFile(filename, "/Users/lhoerger/Documents/curriculum/webserv/ft_webserv/www/root/", "http://localhost:8080/" + filename, envp, true);
					//MyFile myFile(filename, "/home/hoerger/Desktop/ecole 42/ft_webserv/www/root/", envp);
					all_lines = myFile.read_file();
					//system("leaks a.out");
					//all_lines = check_file(filename, "www/root/", envp);
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
				//}
				//else
				//{
				//	return_req.assign(hello, strlen(hello));
				////std::cout << "return req: " << hello << std::endl;
				//write(new_socket , hello , strlen(hello));
				//}
				printf("------------------Hello message sent-------------------");
				close(new_socket);
			//}
		 //}
	}
	return 0;
}