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
bool client_leave(const char* sessionName,  const char *userName);
bool client_exit(const char *userName);
void get_list(int sock);

void add_node();


static userinfo * CurrClient = NULL;
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
    server.sin_port = htons( 8001 );

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
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        else 
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
    bool exit = false;
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
				if(client_exit(packetFromClient.source)){
					printf("Client has some sessions before, now can exit\n");
					
				}
				else{
					printf("Client doesn't have any session, exit directly \n");
				
				}
				exit = true;		//set the flag to break while loop to exit
				break;
				
				
				
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
				if(client_leave(packetFromClient.data, packetFromClient.source)){
					printf("client successfully left session \n");
				
				}
				else{
					printf("cannot find the client in the session \n");				
				}
				break;
				
				
				
			case NEW_SESS:					//case for creating new session
				if(create_new_session(packetFromClient.data, packetFromClient.source, sock)){
					printf("the client just start a new session \n");
					packetToClient.type = NS_ACK;
					strcpy(packetToClient.data,packetFromClient.data);
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
			    get_list(sock);
				break;
			
		
		
		
		
		
		
		
		}
		
		if(exit)
			break;
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
	bool broadcast = false;
	strcpy(info, user);
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
					printf("found the other client, now broadcast  |%s|  \n", head->userName);
					struct lab3message packetToClient;
					packetToClient.type = MESSAGE;
					strcpy(packetToClient.data,message);
					printf("what's the client's socket? %d\n",head-> sock);
					write(head-> sock , &packetToClient , sizeof(packetToClient));
					broadcast = true;
				}
				head=head->next;
			}
			
		}
		temp= temp->next;
	}
	
	if(broadcast)
		return true;
	return false;

}



bool Read_login(const char*username, const char *passwd){
	printf("inside login, username and passwd?  |%s|   |%s|\n",username, passwd);
	FILE* fp;
	fp = fopen("login.txt", "r");
	char * line = NULL;
    size_t len = 0;
    size_t read;
    
    /*first insert this client name into client node ------------*/
    userinfo * head = CurrClient;
    while(head->next!=NULL)
		head = head->next;
	userinfo *newuser = (userinfo *)malloc(sizeof(userinfo));
	strcpy(newuser->userName, username);
	newuser->sock = -1;
	head->next = newuser;			//append newuser to the end of linkelist
	//------------------------------------------------------------
    
    
    
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
	printf("what's the required session name:   |%s| \n", sessionName);
	session * temp =Shead;
	while(temp!=NULL){
		printf("current session name on iteration   |%s| \n", temp->sessionName);
		if(strcmp(temp->sessionName,sessionName)==0){
			printf("found the session\n");
			
			userinfo *head = temp->head;
			if(head ==NULL){
				userinfo *newuser = (userinfo *)malloc(sizeof(userinfo));
				strcpy(newuser->userName,userName);
				newuser->sock = sock;
				head = newuser;
				temp->head = newuser;
				return true;
			
			}
			
			userinfo *newuser = (userinfo *)malloc(sizeof(userinfo));
			strcpy(newuser->userName,userName);
			newuser->sock = sock;
			
			while(head->next!=NULL){
				head = head->next;
			}
			head->next = newuser;
			
			userinfo * usertemp = temp->head;
			while(usertemp!=NULL){
				printf("the current user on this session:  |%s| \n", usertemp->userName);
				usertemp = usertemp->next;
			
			}
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
		printf("head is NULL now: \n");
		session *newsession = (session *)malloc(sizeof(session));
		strcpy(newsession->sessionName,sessionName);
		temp = newsession;
		Shead = temp;
		userinfo *newuser = (userinfo *)malloc(sizeof(userinfo));
		strcpy(newuser->userName,userName);
		newuser->sock = sock;
		newsession->head = newuser;
		printf("session right now? |%s|  |%s|\n",temp->sessionName,Shead->sessionName);
		return true;
	}
	
	while(temp!=NULL){
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
	
	
	printf("session right now? |%s|\n",Shead->sessionName);
	return true;
	

}


bool client_leave(const char* sessionName,  const char *userName){
	session * temp =Shead;
	if(temp == NULL){
		printf("head is NULL now: \n");
		return false;
	}
	while(temp!=NULL){
		if(strcmp(temp->sessionName,sessionName)==0)
		{
			userinfo *head = temp->head;
			userinfo *prev = NULL;
			userinfo *next = head->next;
			int count = 0;
			while(head!= NULL){
				if(strcmp(head->userName, userName)==0){
					printf("found the client, now delete this node from linkedlist \n");
					if(count ==0){		//indicate the client to delete is the head
						printf("I am on the first node to be deleted \n");
						temp->head = NULL;
						temp->head = next;
						free(head);
						//return true;
					}
					
					else{				//case for deleting the node in middle or the end
						prev->next = head->next;
						free(head);
						
					}
					
					//free(socket_desc);
					return true;
					
				}
				prev = NULL;			//reset the previous node pointer
				prev = head;			//assign the old node address to "prev"
				count ++;				//counter keeps track of how many node has went through
				head = head->next;		//move to the next node
			}
			return false;
		
		
		}
		temp = temp->next;
	}
}


bool client_exit(const char *userName){
		session * temp =Shead;
		userinfo * Chead = CurrClient;
		bool has = false;		//indicate whether found this client in any session
		
		userinfo *head = Chead;
		userinfo *prev = NULL;
		userinfo *next = Chead->next;
		
		/*first delete client from the Client linkedlist -----------*/
		while(head !=NULL){
			int count = 0;
			next = head->next;
			if(strcmp(head->userName, userName)==0){
					if(count ==0){
						printf("Find the quitting client, it's at the head of CurrClient   %s \n",head->userName);
						//Chead->head = NULL;
						CurrClient = next;
						free(head);
					}
					else{				//case for deleting the node in middle or the end
							prev->next = head->next;
							head->next = NULL;
							free(head);
						
						}
					break;			//deleted it, now break

			}
			prev = NULL;			//reset the previous node pointer
			prev = head;			//assign the old node address to "prev"
			count ++;				//counter keeps track of how many node has went through
			head = head->next;		//move to the next node

		}
		head = NULL;prev = NULL; next = NULL;
		//----------------------------------------------------------
		
		
		if(temp == NULL){
			printf("head NULL, so client not in any session, can exit directly \n");
			return false;
		}
		
		while(temp!=NULL){
			int count = 0;
			head = temp->head;
			prev = NULL;
			next = head->next;
			
			while(head!= NULL){
				next = head->next;
				if(strcmp(head->userName, userName)==0){
					if(count ==0){
						printf("Find this exiting client in the head of this session   %s \n",temp->sessionName);
						temp->head = NULL;
						temp->head = next;
						free(head);
					}
					else{				//case for deleting the node in middle or the end
							prev->next = head->next;
							head->next = NULL;
							free(head);
						
						}
					has = true;			//found this client in some session(s)
					break;				//same client should not appears twice in the same session, so simply break
				}
				
				prev = NULL;			//reset the previous node pointer
				prev = head;			//assign the old node address to "prev"
				count ++;				//counter keeps track of how many node has went through
				head = head->next;		//move to the next node
			
			}
			temp = temp->next;
		}
		
		if(has)
			return true;
			
		return false;

}


void get_list(int sock){
	struct lab3message packetToClient;
	packetToClient.type = QU_ACK;
	
	
	session * temp =Shead;
	userinfo * Chead = CurrClient;
	while(temp!= NULL){				//copy session names into the first chunk of data
		strcat(packetToClient.data,temp->sessionName);
		strcat(packetToClient.data,":");
		
		temp = temp->next;
	}
	strcat(packetToClient.data,"-");
	
	
	while(Chead!=NULL){
		strcat(packetToClient.data,Chead->userName);
		strcat(packetToClient.data,":");
		Chead = Chead->next;
	}
	//now cat client names
	
	
	
	write(sock , &packetToClient , sizeof(packetToClient));
}

