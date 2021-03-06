
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
char curr_sess[99][2000];



int main(int argc , char *argv[])
{
    

    puts("Welcome, don't forget to /login first !");

    
    struct sockaddr_in server;
   
     char message[1000], message_tmp[1000], msg_without_command[1000] , server_reply[2000], command[2000], ip_addr[100], buffer[1000];
    char arg1[1000], arg2[1000], arg3[1000], arg4[1000];
	memset(message, 0, sizeof message);
	memset(message_tmp, 0, sizeof message_tmp);
	memset(msg_without_command, 0, sizeof msg_without_command);
	memset(server_reply, 0, sizeof server_reply);
	memset(command, 0, sizeof command);
	memset(ip_addr, 0, sizeof ip_addr);
	memset(buffer, 0, sizeof buffer);
	memset(arg1, 0, sizeof arg1);
	memset(arg2, 0, sizeof arg2);
	memset(arg3, 0, sizeof arg3);
	memset(arg4, 0, sizeof arg4);



    //keep communicating with server
    while(1)
    {
	memset(message,0,sizeof message);
	memset(arg1,0,sizeof arg1);
	int i = 0;
        
   	fgets(message, sizeof(message),stdin);

	for(i = 0;i <strlen(message)-1;i++){
		message_tmp[i]=message[i];
	}
	message_tmp[i]='\0';


    for(i = 0;i <strlen(message)-1;i++){
        msg_without_command[i]=message[i];
    }
    msg_without_command[i]='\0';
        
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
	    	puts("CONNECTED");

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
			puts("LOGIN SUCCESSFUL");
			
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


		command_handler(command, arg1, arg2, arg3, arg4, message_tmp, msg_without_command);

		
	}
        
        

    
    }

    close(sock);
    return 0;
}





void command_handler(char* command, char* arg1, char* arg2, char* arg3, char* arg4, char* message, char* msg_without_command){

	
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
		int i;
		for (i = 0; i<(sizeof curr_sess/ sizeof curr_sess[0]);i++)
		{
			if (strlen(curr_sess[i]) != 0)
			{
				strcpy(curr_sess[i], "");
				
			}
		}
		//strcpy(curr_sess,"");
        logged_in = 0;


	}
	else if (!strcmp(command,"/joinsession"))
	{
		int i = 0;        	
		
		bool already_in_sess = false;
		


		for (i = 0; i<(sizeof curr_sess/ sizeof curr_sess[0]);i++)
		{
			if (strcmp(curr_sess[i], arg1) == 0)
			{
				already_in_sess = true;
				break;
			}
		}




		if (!already_in_sess)
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
		else
		{
		    printf("PLESE LEAVE THE CURRENT SESSION FIRST\n");
		}


	}
	else if (!strcmp(command,"/leavesession"))
	{

		int i = 0;        	
		
		bool already_in_sess = false;
		for (i = 0; i<(sizeof curr_sess/ sizeof curr_sess[0]);i++)
		{
			if (strcmp(curr_sess[i], arg1) == 0)
			{
				already_in_sess = true;
				strcpy(curr_sess[i],"");
				break;
			}
		}

		
		

		if (already_in_sess)
		{
		struct lab3message packettoserver;
			packettoserver.type = LEAVE_SESS;
		
		    
		    //printf("%s\n", new_buffer);
			strcpy(packettoserver.source, client_id);
			strcpy(packettoserver.data , arg1);
			packettoserver.size = sizeof(packettoserver.data);


			//Send some data
			if( send(sock , &packettoserver , sizeof(packettoserver) , 0) < 0)
			{
			    puts("Send failed");
			}
			
		}
		else
		{
		    printf("PLESE JOIN A SESSION FIRST\n");
		}


	}
	else if (!strcmp(command,"/createsession"))
	{
		int i = 0;        	
		
		bool already_in_sess = false;
		
		for (i = 0; i<(sizeof curr_sess/ sizeof curr_sess[0]);i++)
		{
			if (strcmp(curr_sess[i], arg1) == 0)
			{
				already_in_sess = true;
				break;
			}
		}

		if (!already_in_sess)
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
		}else
		{
			printf("PLESE LEAVE THE CURRENT SESSION FIRST\n");
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

        exit(0);




	}
	else if (command[0]=='@')
	{
		int i = 0;        	
		
		bool already_in_sess = false;
		for (i = 0; i<(sizeof curr_sess/ sizeof curr_sess[0]);i++)
		{
			if (strlen(curr_sess[i]) != 0)
			{
				already_in_sess = true;
				break;
			}
		}

        
		if (already_in_sess)
		{
			char sess_name[2000];
			char * temp =command;
			strcpy(sess_name, temp+1);
			

			struct lab3message packettoserver;
			packettoserver.type = MESSAGE;


			char new_buffer[2000];
			strcpy (new_buffer, client_id);
			strcat (new_buffer, ":");
			strcat (new_buffer, sess_name);
			
			strcpy(packettoserver.source, new_buffer);


			char msg[2000];
			char* token = strtok(msg_without_command, " ");
				
				int counter = 0;
				while (token != NULL){
				    //printf( "%s \n",token);
				    if (counter ==1)
				        strcpy(msg,token);
				    token = strtok (NULL, " ");
				    counter++;
				}
				counter = 0;



			strcpy(packettoserver.data , msg);








			//printf("packettoserver.source---------%s\n", new_buffer);
			//printf("packettoserver.data---------%s\n", msg);
			packettoserver.size = sizeof(packettoserver.data);
			 //printf("%s\n", packettoserver.data);
		
			//Send some data
			if( send(sock , &packettoserver , sizeof(packettoserver) , 0) < 0)
			{
			    puts("Send failed");
			}


		}
		else
		{
		    printf("PLESE JOIN A SESSION FIRST\n");
		}



	}else
	{
		printf("Unknown command\n");
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
			int i;
			for (i = 0; i<(sizeof curr_sess/ sizeof curr_sess[0]);i++)
				{
					if (strlen(curr_sess[i]) == 0)
					{
						strcpy(curr_sess[i],packetfromserver.data);
						break;
					}
				}


			
			}
			 
			if(packetfromserver.type==JN_NAK)		
			 printf("JOIN SESSION FAILED\n");
			
			if(packetfromserver.type==QU_ACK)		
			 {
			 	printf("GOT LIST, PRINTING\n");
			 
        
				char test_list[2000], list_of_sesssion[2000], list_of_clients[2000];
                 char null[2000]="null";
                 
				//strcpy(test_list , "SessionName1:SessionName2:SessionName3-UserName1:UserName2:UserName3\n");
				//printf ("%s",test_list);
				char* token = strtok(packetfromserver.data, "-");
				
				int counter = 0;
				while (token != NULL){
				    //printf( "%s \n",token);
				    if (counter ==0)
				        strcpy(list_of_sesssion,token);
				    if (counter ==1)
				        strcpy(list_of_clients,token);
				    token = strtok (NULL, " ");
				    counter++;
				}
				counter = 0;
				
				
				int i=0;
				while (list_of_sesssion[i] != '\0'){
				    if (list_of_sesssion[i] == ':') {
				        list_of_sesssion[i] = '\n';
				    } else {
				        //putchar(list_of_sesssion[i]);
				    }
				    i++;
				}
				i=0;
				while (list_of_clients[i] != '\0'){
				    if (list_of_clients[i] == ':') {
				        list_of_clients[i] = '\n';
				    } else {
				        //putchar(list_of_sesssion[i]);
				    }
				    i++;
				}
                 
                 
                 
				if (!strcmp(list_of_sesssion, null))
				printf ("----------------LIST OF SESSIONS----------------\n");
                else
                printf ("----------------LIST OF SESSIONS----------------\n%s\n",list_of_sesssion);
                 
                 
				printf ("----------------LIST OF CLIENTS----------------\n%s\n",list_of_clients);
                
				
				}



			if(packetfromserver.type==NS_ACK)		
			{
                		puts("CREATE SESSION SECCESS");
             
		       		int i;
				for (i = 0; i<(sizeof curr_sess/ sizeof curr_sess[0]);i++)
					{
						
						if (strlen(curr_sess[i]) == 0)
						{
							strcpy(curr_sess[i],packetfromserver.data);
							
							break;
						}
						
					}

				
			
			
               
			}	





			if(packetfromserver.type==MESSAGE)		
			{
			printf("%s ",packetfromserver.source);
			printf("%s\n",packetfromserver.data );
			
			}	
			
			

		}




}

