#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include<sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#define ll long long

#define PORT 8080

int main()
{
	struct sockaddr_in address; 
	int opt=1;
	int addrlen = sizeof(address);

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd== 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				&opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	int new_socket;
	address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
	address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
	address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address,
				sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Port bind is done. You want to wait for incoming connections and handle them in some way.
	// The process is two step: first you listen(), then you accept()
	if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	// returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
					(socklen_t*)&addrlen))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}



	printf("Client is connected to the server!\n\n");
	char *buf = (char *)malloc(sizeof(char)*10000);

	int ret;
	char acknowledegement[]="OKAY";
	while(1)
	{

		//memset(buf,'\0',sizeof(buf));

		ret=read(new_socket,buf,10000);
		buf[ret]='\0';
		send(new_socket,acknowledegement,4,0);


		if (strcmp(buf, "get") == 0)
		{
			while(1)
			{
				//memset(buf,'\0',sizeof(buf));
				ret = read(new_socket,buf,10000);
				buf[ret]='\0';
				//char *acknowledegement2 = (char *)malloc(sizeof(char)*100);

				//acknowledegement2="OKAY";
				send(new_socket,acknowledegement,4,0);
				ret=read(new_socket,acknowledegement,4);
				acknowledegement[ret]='\0';
				if(strcmp(buf,"-1")==0)
					break;
				//printf("debug:%s\n",buf );
				int fd = open(buf,O_RDONLY);
				//printf("debug:%d\n", fd);
				if(fd<0)
				{
					printf("FILE %s does not exist\n", buf);
					char size_send[100];
					sprintf(size_send,"%d",-1);
					send(new_socket , size_send, strlen(size_send) , 0 );
					//char *acknowledegement3 = (char *)malloc(50*sizeof(char));
					ret=read(new_socket,acknowledegement,4);
					acknowledegement[ret]='\0';
					send(new_socket,acknowledegement,4,0);
					continue;
				}
				printf("Started sending file %s\n",buf);
				struct stat p;
				stat(buf,&p);

				int totalsize = p.st_size;

				char size_send[100];
				sprintf(size_send,"%d",totalsize);
				send(new_socket , size_send, strlen(size_send) , 0 );
				//char *acknowledegement4 = (char *)malloc(50*sizeof(char));
				ret=read(new_socket,acknowledegement,4);
				acknowledegement[ret]='\0';
				long long int offset;
				long long int N=10000;
				char *buffer = (char *)calloc(10002,sizeof(char));
				while(totalsize > 0)
				{
					if(totalsize>N)
						offset=N;
					else
						offset=totalsize;
					ret=read(fd, buffer, offset);
					buffer[ret]='\0';
					send(new_socket , buffer , offset , 0 );
					//char *acknowledegement5 = (char *)malloc(50*sizeof(char));
					read(new_socket,acknowledegement,4);
					totalsize=totalsize-offset ;
				}
                printf("request of file %s successfully completed\n\n", buf);
				send(new_socket,acknowledegement,4,0);
				close(fd);
				free(buffer);
			}
		}
		else if (strcmp(buf, "exit") == 0)
		{
			break;
		}
		else
		{
			continue; 
		}
	}
	free(buf);
	close(server_fd);
	close(new_socket);
	return 0;
}

