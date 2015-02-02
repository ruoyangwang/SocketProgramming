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


	



	FILE * fd = fopen("test.png", "rb");
	size_t n, m;
	fseek(fd, 0L, SEEK_END);
	size_t filesize = ftell(fd);
	unsigned char message[BUFLEN];
	unsigned int total_frag = filesize/BUFLEN;
	unsigned int frag_no = 1;
	char *filename = "test.png";
	
	if(filesize%BUFLEN!=0)
		total_frag= filesize/BUFLEN+1;
	//--------------------------------------------------------------------------------------
	
    //--------------------------------------------------------------------------------------
	unsigned char buff[1024];
	do {
	    n = fread(buff, 1, sizeof buff, fd);
	    //if (n) m = fwrite(buff, 1, n, exeout);
	    char newdata[1024];
		char totalfrag[100], curfrag[100];
		char stringinfo[1024];
		sprintf(stringinfo, "%d:%d:%d:%s:",total_frag,frag_no,sizeof (buff),filename);
		//printf("print the string %s: ",stringinfo);
	    if (n) sendto(s, stringinfo, sizeof stringinfo , 0 , (struct sockaddr *) &si_other, slen);
	    else   m = 0;
	} while ((n > 0));
	if (fclose(fd)) perror("close input file");
	


/*
	for (i =0; i<3;i++){
 	char message[BUFLEN] = "3:";  
	char str[3];
	snprintf(str, 3, "%d", 3-i);
    	strcat (message,str);
	strcat (message,":10:test.txt:010101");
        //send the message
        if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
         
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
        {
            die("recvfrom()");
        }
         
        puts(buf);
    	}
 */
    close(s);
    return 0;
}
