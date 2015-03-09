#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <stdbool.h> 
#include "client.h"


bool logged_in = 0;
char client_id[1000] ;
char client_pw[1000] ; 
char server_ip[1000] ;
char server_port[1000] ;
int sock;



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
	strncpy(message_tmp, message, strlen(message));

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

		if(packetfromserver.type=LO_ACK)		
			logged_in = 1;
		if(packetfromserver.type=LO_NAK)		
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

	
	if (command == "/logout")
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


		//Receive a reply from the server
		struct lab3message packetfromserver;
		if( recv(sock , &packetfromserver , sizeof(packetfromserver) , 0) < 0)
		{
		    puts("recv failed");
		}




	}
	else if (command == "/joinsession")
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


		//Receive a reply from the server
		struct lab3message packetfromserver;
		if( recv(sock , &packetfromserver , sizeof(packetfromserver) , 0) < 0)
		{
		    puts("recv failed");
		}

		if(packetfromserver.type=JN_ACK)		
			 printf("JOIN SESSION SECCESS\n");
		if(packetfromserver.type=JN_NAK)		
			 printf("JOIN SESSION FAILED\n");


	}
	else if (command == "/leavesession")
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


		//Receive a reply from the server
		struct lab3message packetfromserver;
		if( recv(sock , &packetfromserver , sizeof(packetfromserver) , 0) < 0)
		{
		    puts("recv failed");
		}



	}
	else if (command == "/createsession")
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


		//Receive a reply from the server
		struct lab3message packetfromserver;
		if( recv(sock , &packetfromserver , sizeof(packetfromserver) , 0) < 0)
		{
		    puts("recv failed");
		}

		if(packetfromserver.type=NS_ACK)		
			 printf("CREATE NEW SESSION SECCESS\n");
		




	}
	else if (command == "/list")
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


		//Receive a reply from the server
		struct lab3message packetfromserver;
		if( recv(sock , &packetfromserver , sizeof(packetfromserver) , 0) < 0)
		{
		    puts("recv failed");
		}

		if(packetfromserver.type=QU_ACK)		
			 printf("GOT LIST, PRINTING\n");

	}
	else if (command == "/quit")
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


		//Receive a reply from the server
		struct lab3message packetfromserver;
		if( recv(sock , &packetfromserver , sizeof(packetfromserver) , 0) < 0)
		{
		    puts("recv failed");
		}





	}
	else
	{
	    	struct lab3message packettoserver;
		packettoserver.type = MESSAGE;
		strcpy(packettoserver.source, client_id);
		//strcpy(packettoserver.data , client_pw);
		packettoserver.size = sizeof(packettoserver.data);


		//Send some data
		if( send(sock , &packettoserver , sizeof(packettoserver) , 0) < 0)
		{
		    puts("Send failed");
		}


		//Receive a reply from the server
		struct lab3message packetfromserver;
		if( recv(sock , &packetfromserver , sizeof(packetfromserver) , 0) < 0)
		{
		    puts("recv failed");
		}





	}
	




}
