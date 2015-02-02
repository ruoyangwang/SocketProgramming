/*
    Simple udp server
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define BUFLEN 1024  //Max length of buffer
#define PORT 8000   //The port on which to listen for incoming data

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
    struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
    unsigned char buf[BUFLEN];
     
    //create a UDP socket
    //IPPROTO_UDP
    if ((s=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
     



    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);
         
        //try to receive some data, this is a blocking call
       	if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
        {
            die("recvfrom()");
        }
         
        //puts(buf);
         
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        //printf("Data: %s\n" , buf);
         

	/*
	//parsing
	struct packet P;
	int count = 0;
	char *total_frag = ""; 
	char *frag_no = ""; 
	char *size = ""; 
	char *filename = ""; 

	for (i=0;i <1024;i++)
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

	printf("%d,%d,%d,%s\n" , P.total_frag, P.frag_no , P.size, P.filename );

*/




	//start creating files
	FILE *fp = fopen("test_copy.png", "ab");
	//fprintf(fp, "%s", buf);
	
	fwrite(buf, sizeof buf, 1, fp);
	

	fclose(fp);







        //now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            die("sendto()");
        }
    }
 
    close(s);
    return 0;
}
