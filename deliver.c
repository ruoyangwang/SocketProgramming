/*
    Simple udp client
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define SERVER "127.0.0.1"
#define BUFLEN 1024  //Max length of buffer
#define PORT 8000   //The port on which to send data
 
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
 
int main(void)
{
    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    char buf[BUFLEN];
    
    //IPPROTO_UDP
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }


	



	
	size_t n=1;
	char *filename = "test.png";
	FILE * fd = fopen("test.png", "rb");


	unsigned int frag_no = 1;
	fseek(fd, 0L, SEEK_END);
	size_t filesize = ftell(fd);
	unsigned int total_frag = filesize/BUFLEN;
	if(filesize%BUFLEN!=0)
		total_frag= filesize/BUFLEN+1;
	fclose(fd);
	fd = fopen("test.png", "rb");
	



	unsigned char buff[1024];
	while (1) {
	   	n = fread(buff, 1, sizeof buff, fd);
		if(n==0)
			break;
		char stringinfo[1024];
		
		sprintf(stringinfo, "%d:%d:%d:%s:",total_frag,frag_no,n,filename);
		int len = strlen(stringinfo)+1+n;
		int sizeOfHeader = strlen(stringinfo)+1;
		char sendbuf[len];
		memcpy(sendbuf,stringinfo, sizeOfHeader);
		memcpy(sendbuf+sizeOfHeader, buff, n);

		//printf("print size of binary data %d    %d  \n", (sendbuf+sizeOfHeader),n);
		frag_no++;
		
	    	sendto(s, buff, sizeof buff , 0 , (struct sockaddr *) &si_other, slen);
	} 
	if (fclose(fd)) perror("close input file");
	frag_no = 0;


    close(s);
    return 0;
}
