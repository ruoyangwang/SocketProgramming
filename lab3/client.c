#include<stdio.h> //printf
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <stdbool.h> 
#include "client.h"
#include<pthread.h> //for threading , link with lpthread


bool logged_in = 0;
char client_id[1000] ;
char client_pw[1000] ; 
char server_ip[1000] ;
char server_port[1000] ;
int sock;
char curr_sess[2000];



int main(int argc , char *argv[])
{
    

    puts("Welcome, don't forget to /login first !");

    
    struct sockaddr_in server;
    char message[1000], message_tmp[1000] , server_reply[2000], command[2000], ip_addr[100], buffer[1000];
    char arg1[1000], arg2[1000], arg3[1000], arg4[1000];

	






    //keep communicating with server
    while(1)
    {

        printf("Enter command: ");
   	fgets(message, sizeof(message),stdin);
	strncpy(message_tmp, message, sizeof(message));
	char* token = strtok(message_tmp, " ");
	int counter = 0;
	while (token != NULL){
		//printf( "%s \n",token);
		if (counter ==0)
			strcpy(command,token);
		if (counter ==1)
			strcpy(arg1,token);
		if (counter ==2)
			strcpy(arg2,token);
		if (counter ==3)
			strcpy(arg3,token);
		if (counter ==4)
			strcpy(arg4,token);
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

		
		strcpy(client_id,arg1);
		strcpy(client_pw,arg2); 
		strcpy(server_ip,arg3);
		strcpy(server_port,arg4);

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

		struct lab3message packettoserver;
		packettoserver.type = LOGIN;
		strcpy(packettoserver.source, client_id);
		strcpy(packettoserver.data , client_pw);
		packettoserver.size = sizeof(packettoserver.data);


		//memcpy(buffer,&packet,sizeof(buffer));
		
		 if( send(sock , &packettoserver , sizeof(packettoserver) , 0) < 0)
		{
		    puts("Send failed");
		    return 1;
		}


		struct lab3message packetfromserver;
		//Receive a reply from the server
		if( recv(sock , &packetfromserver , sizeof(packetfromserver) , 0) < 0)
		{
		    puts("recv failed");
		    break;
		}

		if(packetfromserver.type==LO_ACK){		
			logged_in = 1;

			
			  pthread_t listener_thread;
			if( pthread_create( &listener_thread , NULL ,  listener , NULL) < 0)
			{
			    perror("could not create thread");
			    return 1;
			}
		}

		if(packetfromserver.type==LO_NAK)		
			 printf("LOGIN FAILED\n");


		
	
		
	
	}
	else
	{


		command_handler(command, arg1, arg2, arg3, arg4, message);

		
	}
        
        

    
    }

    close(sock);
    return 0;
}





void command_handler(char command[2000], char arg1[2000], char arg2[2000], char arg3[2000], char arg4[2000], char message[2000]){

	
	if (!strcmp(command,"/logout"))
	{
		struct lab3message packettoserver;
		packettoserver.type = EXIT;
		strcpy(packettoserver.source, client_id);
		//strcpy(packettoserver.data , client_pw);
		packettoserver.size = sizeof(packettoserver.data);

	    	//Send some data
		if( send(sock , &packettoserver , sizeof(packettoserver) , 0) < 0)
		{
		    puts("Send failed");
		}


		strcpy(curr_sess,"");



	}
	else if (!strcmp(command,"/joinsession"))
	{
	    	struct lab3message packettoserver;
		packettoserver.type = JOIN;
		strcpy(packettoserver.source, client_id);
		strcpy(packettoserver.data , arg1);
		packettoserver.size = sizeof(packettoserver.data);


		//Send some data
		if( send(sock , &packettoserver , sizeof(packettoserver) , 0) < 0)
		{
		    puts("Send failed");
		}



	}
	else if (!strcmp(command,"/leavesession"))
	{
	    	struct lab3message packettoserver;
		packettoserver.type = LEAVE_SESS;
		strcpy(packettoserver.source, client_id);
		//strcpy(packettoserver.data , arg1);
		packettoserver.size = sizeof(packettoserver.data);


		//Send some data
		if( send(sock , &packettoserver , sizeof(packettoserver) , 0) < 0)
		{
		    puts("Send failed");
		}
		strcpy(curr_sess,"");

	}
	else if (!strcmp(command,"/createsession"))
	{
	    	struct lab3message packettoserver;
		packettoserver.type = NEW_SESS;
		strcpy(packettoserver.source, client_id);
		strcpy(packettoserver.data , arg1);
		packettoserver.size = sizeof(packettoserver.data);


		//Send some data
		if( send(sock , &packettoserver , sizeof(packettoserver) , 0) < 0)
		{
		    puts("Send failed");
		}

	



	}
	else if (!strcmp(command,"/list"))
	{
	    	struct lab3message packettoserver;
		packettoserver.type = QUERY;
		strcpy(packettoserver.source, client_id);
		//strcpy(packettoserver.data , arg1);
		packettoserver.size = sizeof(packettoserver.data);
		

		//Send some data
		if( send(sock , &packettoserver , sizeof(packettoserver) , 0) < 0)
		{
		    puts("Send failed");
		}


	}
	else if (!strcmp(command,"/quit"))
	{
	    	struct lab3message packettoserver;
		packettoserver.type = EXIT;
		strcpy(packettoserver.source, client_id);
		//strcpy(packettoserver.data , client_pw);
		packettoserver.size = sizeof(packettoserver.data);



		//Send some data
		if( send(sock , &packettoserver , sizeof(packettoserver) , 0) < 0)
		{
		    puts("Send failed");
		}






	}
	else
	{
	    	struct lab3message packettoserver;
		packettoserver.type = MESSAGE;
		char new_buffer[2000];
		strcpy (new_buffer, client_id);
		strcat (new_buffer, ":");
		strcat (new_buffer, curr_sess);
		strcpy(packettoserver.source, new_buffer);

		
		//strcpy(packettoserver.data , client_pw);
		packettoserver.size = sizeof(packettoserver.data);
		

		//Send some data
		if( send(sock , &packettoserver , sizeof(packettoserver) , 0) < 0)
		{
		    puts("Send failed");
		}


		




	}
	




}

















void *listener(){

		
		int read_size;
		struct lab3message packetfromserver;
		 while( (read_size = recv(sock , &packetfromserver , sizeof(packetfromserver) , 0)) > 0 )
		{

			//printf("packet received : %d\n" , packetfromserver.type);


			if(packetfromserver.type==JN_ACK){
			printf("JOIN SESSION SECCESS\n");
			strcpy(curr_sess,packetfromserver.data);
			}		
			 
			if(packetfromserver.type==JN_NAK)		
			 printf("JOIN SESSION FAILED\n");
			
			if(packetfromserver.type==QU_ACK)		
			 printf("GOT LIST, PRINTING\n");


			if(packetfromserver.type==NS_ACK)		
			{
			printf("CREATE SESSION SECCESS\n");
			strcpy(curr_sess,packetfromserver.data);
			}	

		}




}
