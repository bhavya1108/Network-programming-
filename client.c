#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <fcntl.h>
#include <arpa/inet.h>

#define ll long long
#define PORT 8080

int main()
{
	int sockfd, nBytes; 
	struct sockaddr_in addr_con;
	struct sockaddr_in addr_server;
	memset(&addr_server, '0', sizeof(addr_server));    
	addr_server.sin_family = AF_INET; 
	addr_server.sin_port = htons(PORT);
	//FILE* fp; 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd < 0) 
		printf("\nfile descriptor not received!!\n"); 
	else
		printf("\nfile descriptor %d received\n", sockfd); 

	int check1=inet_pton(AF_INET, "127.0.0.1", &addr_server.sin_addr);

	if(check1<=0)
	{
		printf("\nInvalid address\n");
		return -1;
	}
	int check2=connect(sockfd, (struct sockaddr *)&addr_server, sizeof(addr_server));
	if (check2 < 0)  // connect to the server address
	{
		printf("Connection Failed \n");
		return -1;
	}
	printf("Connected to the server successfully :-)\n");

	while(1)
	{
		printf("client> ");
		char *input =(char *)malloc(sizeof(char)*1000);
		size_t input_size=0;

		getline(&input,&input_size,stdin);
		if(strlen(input)==1)
			continue;
		input[strlen(input)-1]='\0';
		//printf("%s\n",input);
		char *tok;
		tok = strtok(input," \t");

		send(sockfd, tok ,strlen(tok), 0);
		int ret;
		char *acknowledegement = (char *)malloc(sizeof(char)*100);
		ret=read(sockfd,acknowledegement,4);
		acknowledegement[ret]='\0';
		char *buf = (char *)malloc(sizeof(char)*10000);


		if(strcmp("get",tok)==0)
		{

			tok=strtok(NULL," \t");
			while(tok!=NULL)
			{
				//printf("debug:%s\n",tok);
				send(sockfd,tok,strlen(tok),0);
				//char *acknowledegement2 = (char *)malloc(50*sizeof(char));
				ret=read(sockfd,acknowledegement,4);
				acknowledegement[ret]='\0';
				send(sockfd,acknowledegement,4,0);

				ret=read(sockfd,buf,500);
				buf[ret]='\0';
				//char *acknowledegement3 = (char *)malloc(sizeof(char)*100);

				//acknowledegement3="OKAY";
				send(sockfd,acknowledegement,4,0);

				int size = atoi(buf);

				if(size == -1)
				{
					printf(" file %s does not exist\n",tok);
					//memset(buf,'\0',sizeof(buf));
					ret=read(sockfd,acknowledegement,4);
					acknowledegement[ret]='\0';
					tok = strtok(NULL," \t");
					continue;
				}

                printf("your request of file %s is being proccessed\n",tok );

				int fd = open(tok,O_WRONLY | O_TRUNC | O_CREAT, 0666);
				long long int N=10000;
				char *buffer = (char *)calloc(10002,sizeof(char));

				long long int offset;

				double sz=size;
				long long int counter=0;
				long long int percent=0;
				//lseek(fd,0,SEEK_SET);
				while(size>0)
				{
					if(size>N)
					{
						offset=N;
						counter++;
						percent=N*counter*100;
					}
					else
					{
						offset=size;
						percent=(sz)*100;

					}
					ret=read(sockfd,buffer,offset);
					buffer[ret]='\0';
					//char *acknowledegement4 = (char *)malloc(sizeof(char)*100);
					//acknowledegement4="OKAY";
					send(sockfd,acknowledegement,4,0);

					//write(fd,buffer,10000);

					write(fd,buffer,offset);

					size-=offset;

					char tmp[100]={0x0};

					float ans=(float)percent/sz;
					//printf("%0.2f\n",ans);
					sprintf(tmp,"%0.2f percent file have been received",ans);

					write(1,"\r",1);
					write(1,tmp,sizeof(tmp));
					fflush(stdout);
					//system("clear");
				}

				ret=read(sockfd,acknowledegement,4);
				acknowledegement[ret]='\0';
				//printf("\n\ndebug:acknowledgement before message is %s\n\n",acknowledegement);
				char *message = (char *)malloc(100*sizeof(char));
				
				sprintf(message, "\nFile Transfer Completed Successfully.............100%% done\n\n");
				write(1, message, strlen(message));
				tok=strtok(NULL," \t");
				//printf("debug: 1 reached\n");
				close(fd);
				free(buffer);
				free(message);//memset(buf, '\0', sizeof(buf));
				//printf("debug: 2 reached\n");
			}
			char toke[]="-1";
			send(sockfd,toke,strlen(toke),0);

			//char *acknowledegement5 = (char *)malloc(50*sizeof(char));
			ret=read(sockfd,acknowledegement,4);
			acknowledegement[ret]='\0';
			send(sockfd,acknowledegement,4,0);
		}
		else if(strcmp("exit",tok)==0)
		{
			printf("Disconnected from the server :-o\n");
			break;
		}
		else 
		{
			printf("Enter correct command\n");
            continue;
		}
		free(buf);
		free(acknowledegement);
		free(tok);
		free(input);
	}
	close(sockfd);
	return 0;
}























