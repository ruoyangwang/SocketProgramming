/*
    Simple udp server
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
 
#define BUFLEN 1000  //Max length of UDP datagram
#define PORT 8000   //The port on which to listen for incoming data

struct packet { 
unsigned int total_frag; 
unsigned int frag_no; 
unsigned int size; 
char* filename; 
char filedata[512]; 
};

 
void die(char *s)
{
    perror(s);
    exit(1);
}
 
int main (int argc, char *argv[])
{
    	
	if ( argc != 2 ) /* argc should be 2 for correct execution */
   	{
        /* We print argv[0] assuming it is the program name */
        printf( "only one arugment allowed" );
		exit;
    	}


    struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
    unsigned char buf[BUFLEN];
    int frag_counter = 1; 
     
    //create a UDP socket
    //IPPROTO_UDP
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(atoi(argv[1]));
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }

    while(1)
    {
        
        fflush(stdout);
        
        //try to receive some data, this is a blocking call
       	if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
        {
            die("recvfrom()");
        }

	//parsing
	struct packet P;
	int count = 0;
	char *total_frag = ""; 
	char *frag_no = ""; 
	char *size = ""; 
	char *filename = "";
	char filedata[512] = ""; 

	for (i=0;i <1000;i++)
	{
		if (buf[i] != ':')
		{
			if (count == 0){
	     			    size_t len = strlen(total_frag);
				    char *str2 = malloc(len + 1 + 1 ); 
				    strcpy(str2, total_frag);
				    str2[len] = buf[i];
				    str2[len + 1] = '\0';

				   
				    total_frag = str2;

			
			}else if (count == 1){

				    size_t len = strlen(frag_no);
				    char *str2 = malloc(len + 1 + 1 ); 
				    strcpy(str2, frag_no);
				    str2[len] = buf[i];
				   str2[len + 1] = '\0';

				
				    frag_no = str2;

			}else if (count == 2){

				    size_t len = strlen(size);
				    char *str2 = malloc(len + 1 + 1 ); 
				    strcpy(str2, size);
				    str2[len] = buf[i];
				   str2[len + 1] = '\0';

				
				    size = str2;

			}else if (count == 3){
				    size_t len = strlen(filename);
				    char *str2 = malloc(len + 1 + 1 ); 
				    strcpy(str2, filename);
				    str2[len] = buf[i];
				   str2[len + 1] = '\0';

				
				    filename = str2;
				    
			}else if (count == 4)
			{
				break;
			}

			
		}else
		{
			count+=1;
			
		}
	}
	count = 0;

	P.total_frag = atoi(total_frag);
	P.frag_no = atoi(frag_no);
	P.size = atoi(size);
	P.filename = filename;

	

	char stringinfo[1024];
	sprintf(stringinfo, "%d:%d:%d:%s:",P.total_frag, P.frag_no , P.size, P.filename);
	int sizeOfHeader = strlen(stringinfo)+1;
	//printf("check binary data size and head size  %d ----- %d  ",P.size, sizeOfHeader);
	printf("%d:%d:%d:%s:\n" , P.total_frag, P.frag_no , P.size, P.filename );

	

	memcpy(P.filedata,buf+sizeOfHeader, P.size);
	printf("filedata--%d Psize-----%d\n", sizeof P.filedata, P.size  );
	char name[100]="cpy_";
	strcat(name,P.filename);
	FILE *fp = fopen(name, "ab");
	
	fwrite(P.filedata, sizeof P.filedata, 1, fp);
	
	fclose(fp);

    }
 
    close(s);
    return 0;
}
