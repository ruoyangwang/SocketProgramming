
/*
    Simple udp client
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
 

#define BUFLEN 1000  //Max length of buffer

 
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



void send_packet(char* filename, struct sockaddr_in* si_other, int s, int slen){
	size_t n=1;
	FILE * fd = fopen(filename, "rb");


	unsigned int frag_no = 1;
	fseek(fd, 0L, SEEK_END);
	size_t filesize = ftell(fd);
	unsigned int total_frag = filesize/BUFLEN;
	if(filesize%BUFLEN!=0)
		total_frag= filesize/BUFLEN+1;
	fclose(fd);
	fd = fopen(filename, "rb");
	
	unsigned char buff[BUFLEN];
	unsigned char ack[100];
	
	
	while (1) {
		/*n represent number of bythes received*/
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

		printf("print size of binary data %d    %d   %d   %d\n", sizeof sendbuf ,sizeof buff,total_frag,frag_no);
		frag_no++;
		
	    sendto(s, sendbuf, sizeof sendbuf , 0 , (struct sockaddr *) si_other, slen);
	    
	    if (recvfrom(s, ack, sizeof ack, 0, (struct sockaddr *) si_other, &slen) == -1)
		{
		        die("recvfrom()");
		}
		else{
			
			char newstring[100];
			strcpy(newstring, ack);
			char* token = strtok(newstring, ":");
			
			char first[10],second[10];
			strcpy(first,token);
			printf( "%s  %s  %s\n",ack,first,second);
			if(atoi(first)==-1)
				sendto(s, sendbuf, sizeof sendbuf , 0 , (struct sockaddr *) si_other, slen);
		}
		
	} 
	if (fclose(fd)) perror("close input file");
	frag_no = 0;

}







int main(int argc, char **argv)
{
 
	char * ip = argv[1];
	int PORT = atoi(argv[2]);
	int listenPortNum = atoi(argv[3]);
	char *filename = argv[4];
	printf("%s %d %d %s \n",ip,PORT,listenPortNum,filename);
    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    
    //IPPROTO_UDP
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_aton(ip, &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

	send_packet(filename, &si_other, s, slen);

    close(s);
    return 0;
}

