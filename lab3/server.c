#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <stdbool.h> 
#include<pthread.h> //for threading , link with lpthread
#include "client.h"

void *connection_handler(void *);
bool Read_login(const char*username, const char *passwd);
bool check_session_exist(const char* sessionName, const char *userName, int sock );
bool create_new_session(const char* sessionName,  const char *userName, int sock);
bool send_message(const char*message, char *user);

void add_node();

static session *Shead = NULL;

int main(int argc , char *argv[])
{


	
    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    char *message;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8000 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }

    if (new_socket<0)
    {
        perror("accept failed");
        return 1;
    }

    return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
    struct lab3message packetFromClient;
	struct lab3message packetToClient;
    //Receive a message from client
    while( (read_size = recv(sock , &packetFromClient , sizeof(packetFromClient) , 0)) > 0 )
    {
		if(read_size == 0)
		{
		    puts("Client disconnected");
		    fflush(stdout);
		}
		else if(read_size == -1)
		{
		    perror("recv failed");
		}
		
		switch(packetFromClient.type){
			case EXIT:						//case for handling exit
				
			
			case LOGIN:					//case for handling login
				printf("login section, mock to send back ACK\n");
				
				
				if(Read_login(packetFromClient.source, packetFromClient.data)){
					packetToClient.type = LO_ACK;
				}
				else
					packetToClient.type= LO_NAK;
					
				write(sock , &packetToClient , sizeof(packetToClient));
					break;
				
			case JOIN:					//case for handling join session
				if(check_session_exist(packetFromClient.data, packetFromClient.source, sock)){
					printf("successfully insert this client into the according session \n");
					packetToClient.type = JN_ACK;
					strcpy(packetToClient.data,packetFromClient.data);
					write(sock , &packetToClient , sizeof(packetToClient));
				}
				else{
					printf("cannot find the session, please create session first \n");
					packetToClient.type = JN_NAK;
					 write(sock , &packetToClient , sizeof(packetToClient));
				}
				break;
				
			case LEAVE_SESS:					//case for leaving new session
				;
				break;
				
				
			case NEW_SESS:					//case for creating new session
				if(create_new_session(packetFromClient.data, packetFromClient.source, sock)){
					printf("the client just start a new session \n");
					packetToClient.type = NS_ACK;
					strcpy(packetToClient.data,"new session created");
					printf("packetfromServer type? %d \n",packetToClient.type);
					write(sock , &packetToClient , sizeof(packetToClient));	
				}
				else{
					printf("cannot create session, session might already exist \n");
					//packetToClient.type = NS_NAK;
					//write(sock , &packetToClient , sizeof(packetToClient));	
				}
				break;
				
				
			case MESSAGE:					//case for sending message
				if(send_message(packetFromClient.data, packetFromClient.source)){
					printf("message successfully broadcasted \n");				
				}
				else
					printf("cannot find the session or user not in the session \n");		
				break;
				
				
			case QUERY:					//case for getting list
				;
				break;
			
		
		
		
		
		
		
		
		}
        //Send the message back to client
        //write(sock , &packetToClient , sizeof(packetToClient));
    }

   
    //Free the socket pointer
    free(socket_desc);

    return 0;
}



bool send_message(const char*message,  char *user){
	char *token;
	char uName[50];
	char sName[50];
	char info[MAX_NAME];
	int i = 0;
	//strcpy(info, user);
	printf("print first info: |%s|  , and the message? : |%s| \n",user, message);
	session * temp =Shead;
	
	token = strtok(info, ":");
	strcpy(uName, token);
   	printf("print userName: %s , leftover string: %s\n", token,info);
	   /* walk through other tokens */
	for(i=0;i<1;i++) 
	{
		  //printf( " %s\n", token );
		
		  token = strtok(NULL, ":");
		  strcpy(sName,token);
	}
	
	printf("session name :  %s  \n",sName);
	
	while(temp!=NULL){
		if(strcmp(temp->sessionName,sName)==0){
			userinfo *head = temp->head;
			while(head!= NULL){
				if(strcmp(head->userName, uName)){
					struct lab3message packetToClient;
					packetToClient.type = MESSAGE;
					strcpy(packetToClient.data,message);
					write(head-> sock , &packetToClient , sizeof(packetToClient));
				}
				head=head->next;
			}
		
		}
		temp= temp->next;
	}

}



bool Read_login(const char*username, const char *passwd){
	printf("inside login, username and passwd?  |%s|   |%s|\n",username, passwd);
	FILE* fp;
	fp = fopen("login.txt", "r");
	char * line = NULL;
    size_t len = 0;
    size_t read;
    
    
	while ((read = getline(&line, &len, fp)) != -1) {
		   char *token;
		   char uName[50];
		   char password[50];
		   char passwd[50];
		   int i = 0;
		   //printf("current line for read: %s \n");
		   
		   token = strtok(line, ":");
		   strcpy(uName, token);
		   for(i=0;i<1;i++) {

				  token = strtok(NULL, ":");
				  strcpy(password,token);
			}
			
			
			for(i = 0; i<strlen(password);i++){
				passwd[i]=password[i];
			}	
			//printf("the username and password after tok? :  |%s|    |%s|\n",uName, passwd);
		   if(strcmp(username,uName)==0){
		   		//printf("find the same username \n");
		   		if(strcmp(passwd,password)==0){
		   			//printf("find the same password, return true \n");
		   			return true;
		   		
		   		}
		   		else
		   			return false;
		   
		   }
		   
    }

}



bool check_session_exist(const char* sessionName,  const char *userName, int sock){
	session * temp =Shead;
	while(temp!=NULL){
		if(strcmp(temp->sessionName,sessionName)==0){
			userinfo *head = temp->head;
			userinfo *newuser = (userinfo *)malloc(sizeof(userinfo));
			strcpy(newuser->userName,userName);
			newuser->sock = sock;
			
			while(head->next!=NULL)
				head = head->next;
			head = newuser;

			return true;
		}
		temp = temp->next;
	}
	return false;

}



bool create_new_session(const char* sessionName,  const char *userName, int sock){
	printf("now create new session, what's the userName:  |%s| , what's session name: |%s| \n",userName, sessionName);
	session * temp =Shead;
	if(temp == NULL){
		session *newsession = (session *)malloc(sizeof(session));
		strcpy(newsession->sessionName,sessionName);
		temp = newsession;
		
		userinfo *newuser = (userinfo *)malloc(sizeof(userinfo));
		strcpy(newuser->userName,userName);
		newuser->sock = sock;
		newsession->head = newuser;
		
		return true;
	}
	
	while(temp->next!=NULL){
		if(strcmp(temp->sessionName,sessionName)==0)
				return false;
		temp = temp->next;
	}
	
	session *newsession = (session *)malloc(sizeof(session));
	strcpy(newsession->sessionName,sessionName);
	temp->next = newsession;
		
	userinfo *newuser = (userinfo *)malloc(sizeof(userinfo));
	strcpy(newuser->userName,userName);
	newuser->sock = sock;
	newsession->head = newuser;
	
	return true;
	

}





