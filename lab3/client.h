#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>

//define struct length	
#define MAX_NAME 2000
#define MAX_DATA 2000




//packet type protocal

#define EXIT 0
#define LOGIN 10
#define LO_ACK 11
#define LO_NAK 12

#define JOIN 20
#define JN_ACK 21
#define JN_NAK 22

#define LEAVE_SESS 50

#define NEW_SESS 60

#define NS_ACK 61


#define MESSAGE 100

#define QUERY 200
#define QU_ACK 201



struct lab3message { 
 unsigned int type; 
 unsigned int size; 
 unsigned char source[MAX_NAME]; 
 unsigned char data[MAX_DATA]; 
};


struct userinfo  {
	char userName[MAX_NAME];
	int sock;
	struct userinfo *next;
};
typedef struct userinfo userinfo;

struct session {
	char sessionName[MAX_NAME];
	userinfo *head;
	struct session *next;
};
typedef struct session session;





void command_handler(char* command, char* arg1, char* arg2, char* arg3, char* arg4, char* message, char* msg_without_command);
void *listener();
#endif
