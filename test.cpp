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

#define PORT 8080


//char	*ft_substr(char const *s, unsigned int start, size_t len)
//{
//	char	*s2;
//	size_t	len_substr;

//	if (!s)
//		return (0);
//	len_substr = strlen(&s[start]);
//	if (len_substr > len)
//		len_substr = len;
//	s2 = malloc(sizeof(char) * len + 1);
//	if (len == 0 || start >= strlen(s))
//	{
//		s2[0] = '\0';
//		return (s2);
//	}
//	if (!s2)
//		return (0);
//	if (start > strlen(s))
//		return (s2);
//	ft_memcpy(s2, &s[start], len_substr);
//	s2[len_substr] = '\0';
//	return (s2);
//}

int main(int argc, char const *argv[])
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
	while(1)
	{
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
		{
			perror("In accept");
			exit(EXIT_FAILURE);
		}
		
		char buffer[30000] = {0};
		valread = read( new_socket , buffer, 30000);
		printf("%s\n",buffer );
		int i = 0;
		while(buffer[i] != '/')
			i++;
		int j = i;
		while(buffer[j] != ' ')
			j++;
		printf("############FILENAME\n");
		i++; // wegen /
		std::cout << "i: " << i << "j: " << j << std::endl;
	
		std::string new_buffer = (std::string) buffer;
		std::string filename = new_buffer.substr(i, j - i);

		std::string return_req;
		if (filename == "index.html")
		{


			std::cout << filename << std::endl << std::endl;
			std::ifstream file;
			std::string line;
			file.open(filename);
			while (getline(file, line))
			{
				std::cout << line << "\n";
			}
			file.close();
			//while (i < j)
			//{
			//	printf("%c", buffer[i]);
			//	i++;
			//}
			//printf("\n\n");
			std::string return_req = header + line;
					std::cout << "return req: " << return_req << std::endl;
		write(new_socket , return_req.c_str() , return_req.length());
		}
		else
		{
			return_req.assign(hello, strlen(hello));
		std::cout << "return req: " << hello << std::endl;
		write(new_socket , hello , strlen(hello));
		}
		printf("------------------Hello message sent-------------------");
		close(new_socket);
	}
	return 0;
}