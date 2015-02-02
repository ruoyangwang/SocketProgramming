/*
    Simple udp client
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>


#define BUFLEN 256  //Max length of buffer

 
struct packet { 
		unsigned int total_frag; 
		unsigned int frag_no; 
		unsigned int size; 
		char* filename; 
		char filedata[1000]; 
	};


void die(char *s)
{
    perror(s);
    exit(1);
}
 
int main(int argc, char *argv[])
{
	char str1[50];
	
    struct sockaddr_in server;
    int s, i, slen=sizeof(server);
    char message[BUFLEN];
 	/*scanf("%s", &str1);
 	char * token = strtok(str1, " ");*/
 	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if ( sock == -1)
    {
        die("socket");
    }
 
    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
     
    if (inet_aton(argv[0] , &server.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }


	/*FILE *img = fopen("test.png", "rb");
	fseek(img, 0, SEEK_END);
	unsigned long filesize = ftell(img);
	char *buffer = (char*)malloc(sizeof(char)*filesize);
	rewind(img);
	// store read data into buffer
	fread(buffer, sizeof(char), filesize, img);

	// send buffer to client
	int sent = 0;
	while (sent < filesize)
	{
		int n = send(clientsocket, buffer + sent, filesize - sent, 0);
		if (n == -1)
		    break;  // ERROR

		sent += n;
	}*/

	FILE * fd = fopen("test.png", "rb");
	size_t filesize = ftell(fd);
	char *buffer = malloc(BUFLEN);
	unsigned int bytes_read = 0;
	int offset = 0;
	unsigned int total_frag = filesize/BUFLEN;
	unsigned int frag_no = 1;
	char *filename = argv[3];
	
	if(filesize%BUFLEN!=0)
		total_frag= filesize/BUFLEN+1;
	
	
	while(bytes_read = fread(buffer,  sizeof(char), BUFLEN, fd)>0)
	{
			
			if(send(sock,buffer,bytes_read,0)>0)
		    {
		        printf("File sent successfully \n");
				/*filesize-=bytes_read;
				offset+=bytes_read;*/
				memset(buffer, 0, sizeof buffer);
		    }
			else{
				die("sendto()");
			}
		
	
	}
 


    close(server);
    return 0;
}
