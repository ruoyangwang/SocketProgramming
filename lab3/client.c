#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <stdbool.h> 


bool logged_in = 0;
#define MAX_NAME 2000
#define MAX_DATA 2000

struct lab3message { 
 unsigned int type; 
 unsigned int size; 
 unsigned char source[MAX_NAME]; 
 unsigned char data[MAX_DATA]; 
};




int main(int argc , char *argv[])
{
    

    puts("Welcome, don't forget to /login first !");

    int sock;
    struct sockaddr_in server;
    char message[1000], message_tmp[1000] , server_reply[2000], command[2000], ip_addr[100];
    char client_id[1000];
    char client_pw[1000]; 
    char server_ip[1000];
    char server_port[1000];


	






    //keep communicating with server
    while(1)
    {

        printf("Enter command: ");
   	fgets(message, sizeof(message),stdin);
	strncpy(message_tmp, message, strlen(message));

	char* token = strtok(message_tmp, " ");
	int counter = 0;
	while (token != NULL){
		//printf( "%s \n",token);
		if (counter ==0)
			strcpy(command,token);
		if (counter ==1)
			strcpy(client_id,token);
		if (counter ==2)
			strcpy(client_pw,token);
		if (counter ==3)
			strcpy(server_ip,token);
		if (counter ==4)
			strcpy(server_port,token);
		token = strtok (NULL, " ");
		counter++;
	}
	counter = 0;





	if (strcmp(command,"/login") != 0 && !logged_in)
	{
		puts("please login first!");
	}
	else if (strcmp(command,"/login") == 0 && logged_in)
	{
		puts("you already logged in!");
	}
	else if (strcmp(command,"/login") == 0 && !logged_in)
	{

		


		//Create socket from client input
	    	sock = socket(AF_INET , SOCK_STREAM , 0);
	    	if (sock == -1)
	    	{
			printf("Could not create socket");
	    	}
	    	puts("Socket created");

	    	server.sin_addr.s_addr = inet_addr(server_ip);
	    	server.sin_family = AF_INET;
	    	server.sin_port = htons( atoi(server_port) );

	    	//Connect to remote server
	    	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	    	{
			perror("connect failed. Error");
			return 1;
	    	}




		 if( send(sock , message , strlen(message) , 0) < 0)
		{
		    puts("Send failed");
		    return 1;
		}

		//Receive a reply from the server
		if( recv(sock , server_reply , 2000 , 0) < 0)
		{
		    puts("recv failed");
		    break;
		}

		logged_in = 1;
		 printf("Server Reply: %s\n", server_reply);
   		 server_reply[0]='\0';
	}
	else
	{

		//Send some data
		if( send(sock , message , strlen(message) , 0) < 0)
		{
		    puts("Send failed");
		    return 1;
		}

		//Receive a reply from the server
		if( recv(sock , server_reply , 2000 , 0) < 0)
		{
		    puts("recv failed");
		    break;
		}

		 printf("Server Reply: %s\n", server_reply);
   		 server_reply[0]='\0';

	}
        
        

    
    }

    close(sock);
    return 0;
}
