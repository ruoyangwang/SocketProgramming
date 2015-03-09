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
int Read_login(const char*username, const char *passwd);
bool check_session_exist(const char* sessionName, const char *userName, int sock );
bool create_new_session(const char* sessionName,  const char *userName, int sock);
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
				if(Read_login(packetFromClient.source, packetFromClient.data)){
					;
				}
				else
					;
				
			case JOIN:					//case for handling join session
				if(check_session_exist(packetFromClient.data, packetFromClient.source, sock)){
					printf("successfully insert this client into the according session \n");
					packetToClient.type = JN_ACK;
					 write(sock , &packetToClient , sizeof(packetToClient));
				}
				else{
					printf("cannot find the session, please create session first \n");
					packetToClient.type = JN_NAK;
					 write(sock , &packetToClient , sizeof(packetToClient));
				}
			case LEAVE_SESS:					//case for leaving new session
				;
			case NEW_SESS:					//case for creating new session
				if(create_new_session(packetFromClient.data, packetFromClient.source, sock)){
					printf("the client just start a new session \n");
					packetToClient.type = JN_ACK;
					write(sock , &packetToClient , sizeof(packetToClient));	
				}
				else{
					printf("cannot create session, session might already exist \n");
					packetToClient.type = JN_NAK;
					write(sock , &packetToClient , sizeof(packetToClient));	
				}
			case MESSAGE:					//case for sending message
				;
			
			case QUERY:					//case for getting list
				;
			
			
		
		
		
		
		
		
		
		}
        //Send the message back to client
        write(sock , &packetToClient , sizeof(packetToClient));
    }

   
    //Free the socket pointer
    free(socket_desc);

    return 0;
}



int Read_login(const char*username, const char *passwd){
	FILE* fp;
	fp = fopen("login.txt", "r");
	char * line = NULL;
    size_t len = 0;
    size_t read;
    
	while ((read = getline(&line, &len, fp)) != -1) {
		    ;
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





