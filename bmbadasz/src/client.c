#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "../include/commands.h"
#include "../include/logger.h"
#include "../include/server.h"

#define CLIENTBUFFERSIZE 256
#define TOKEN_BUFFER_SIZE 64

int clientSock;
struct sockaddr_in server;
bool loggedin = false;

/*Client initialization and logic*/
void init_client(int portNum){
    /*Set up client socket*/
    clientSock = socket(PF_INET, SOCK_STREAM, 0);
}

void populateServerData(char *buffer){
    /*Break up rec'd packet into clients*/
    int client_buffer_size = CLIENTBUFFERSIZE;
	int trav = 0;
	char **clients = malloc(client_buffer_size * sizeof(char*));
    char *client_delimiters = "\n";
	char *client = strtok(buffer, client_delimiters);
	while(client != NULL) {
		clients[trav] = client;
		trav = trav + 1;
		if(trav >= client_buffer_size){
			client_buffer_size += CLIENTBUFFERSIZE;
			clients = realloc(clients, client_buffer_size * sizeof(char*));
		}
	    client = strtok(NULL, client_delimiters);
	}
	clients[trav] = NULL;
    /*Now should have a list of clients*/
    /*Use this info to populate serverData SLIST*/
    int clientTrav = 0;
    client = clients[clientTrav];
    while(client != NULL){
        int token_buffer_size = TOKEN_BUFFER_SIZE;
	    int trav2 = 0;
	    char **tokens = malloc(token_buffer_size * sizeof(char*));
	    char *token = strtok(client, ";");
	    while(token != NULL) {
		    tokens[trav2] = token;
		    trav2 = trav2 + 1;
		    if(trav2 >= token_buffer_size){
			    token_buffer_size += TOKEN_BUFFER_SIZE;
			    tokens = realloc(tokens, token_buffer_size * sizeof(char*));
		    }
	        token = strtok(NULL, ";");
	    }
	    tokens[trav2] = NULL;
        /*Should have split info ready to be loaded into serverData*/
        struct entry *newEnt;
        newEnt = malloc(sizeof(struct entry));
        strcpy(newEnt->hostname, tokens[0]);
        strcpy(newEnt->ip, tokens[1]);
        int porttmp = atoi(tokens[2]);
        newEnt->port = porttmp;
        if(strcmp("true", tokens[3]) == 0){
            newEnt->loggedIn = true;
        } else {
            newEnt->loggedIn = false;
        }
        serverDataInsert(newEnt);
        /*Incrememnt to next client*/
        clientTrav++;
        client = clients[clientTrav];
    }
}

/*Client Functions*/

/**
 * @param portNum Port the client is listening on
 * @param serverIP Must be included as arg, IP of the server
 * @param serverPort Must be included as arg, listening port of server
*/
int login(int portNum, char ** input){
    /*Check to see if there's proper input and stuff*/
    if(input[1]==NULL || (input[1]!=NULL && input[2]==NULL)){
        cse4589_print_and_log("[LOGIN:ERROR]\n");
        printf("LOGIN Usage:\tLOGIN <IP> <PORT>\n");
        cse4589_print_and_log("[LOGIN:END]\n");
        return -1;
    }
    if(input[1]!=NULL){
        int rv;
        regex_t ipv4;
        regmatch_t matches[1]; //Regex garbage
        rv = regcomp(&ipv4, "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$", REG_EXTENDED);
        if(regexec(&ipv4, input[1], 1, matches, 0)!=0){
            cse4589_print_and_log("[LOGIN:ERROR]\n");
            printf("Not a valid IPv4 address\n");
            cse4589_print_and_log("[LOGIN:END]\n");
            return -1;
        }
        regfree(&ipv4);
        if(input[2]!=NULL){
            int rv2;
            regex_t prt;
            rv2 = regcomp(&prt, "^((6553[0-5])|(655[0-2][0-9])|(65[0-4][0-9]{2})|(6[0-4][0-9]{3})|([1-5][0-9]{4})|([0-5]{0,5})|([0-9]{1,4}))$", REG_EXTENDED);
            if(regexec(&prt, input[2], 1, matches, 0)!=0){
                cse4589_print_and_log("[LOGIN:ERROR]\n");
                printf("Not a valid Port Number\n");
                cse4589_print_and_log("[LOGIN:END]\n");
                return -1;
            }
            regfree(&prt);
        }
    }
    char *serverIP = input[1];
    char *serverPort = input[2];

    /*Set up server socket*/
    memset(&server, '\0', sizeof(server));
    server.sin_family=AF_INET;
    int sPortNum = atoi(serverPort);
    server.sin_port=htons(sPortNum);
    server.sin_addr.s_addr=inet_addr(serverIP);
    /*Attempt connection*/
    if(connect(clientSock, (struct sockaddr*)&server, sizeof(server)) < 0){
                cse4589_print_and_log("[LOGIN:ERROR]\n");
                printf("Connection failed\n");
                cse4589_print_and_log("[LOGIN:END]\n");
                return -1;
    }/*Connection established*/

    /*Setting up packet to be sent to server*/
    char hostname[128];
    gethostname(hostname, sizeof(hostname));
    /*You have to call get_own_ip like this because blah*/
    char clientIP[INET_ADDRSTRLEN];
    memset(clientIP, '\0', sizeof(clientIP));
    get_own_ip(clientIP, portNum);
    /*Packet format: Hostname, IP, Port*/
    char loginPacket[1024];
    sprintf(loginPacket, "LOGIN;%s;%s;%d;", hostname, clientIP, portNum);

    /*Send login packet to the server*/    
    char buffer[1024];
    if(send(clientSock, loginPacket, strlen(loginPacket), 0) < 0){
                cse4589_print_and_log("[LOGIN:ERROR]\n");
                printf("Sending failed\n");
                cse4589_print_and_log("[LOGIN:END]\n");
                return -1;
    } /*Send successful*/

    /*Read data returning from server*/
    int valread = read(clientSock, buffer, 1024);
    /*Take data from server, populate info*/
    populateServerData(buffer);
    loggedin = true;
    cse4589_print_and_log("[LOGIN:SUCCESS]\n");
    cse4589_print_and_log("[LOGIN:END]\n");
}

int refresh(){
    /*Check for login*/
    if(!loggedin){
        cse4589_print_and_log("[REFRESH:ERROR]\n");
        printf("Must be logged in!\n");
        cse4589_print_and_log("[REFRESH:END]\n");
        return -1;
    }
    char *refreshPacket = "REFRESH";
    if(send(clientSock, refreshPacket, strlen(refreshPacket), 0) < 0){
                cse4589_print_and_log("[REFRESH:ERROR]\n");
                printf("Sending failed\n");
                cse4589_print_and_log("[REFRESH:END]\n");
    } /*Send successful*/
    char buffer[1024];
    memset(buffer, '\0', sizeof(buffer));
    int valread = read(clientSock, buffer, 1024);
    struct entry *n1;
    while (!SLIST_EMPTY(&head)) {           /* List Deletion. */
            n1 = SLIST_FIRST(&head);
            SLIST_REMOVE_HEAD(&head, entries);
            free(n1);
        }
    SLIST_INIT(&head);
    populateServerData(buffer);
    cse4589_print_and_log("[REFRESH:SUCCESS]\n");
    cse4589_print_and_log("[REFRESH:END]\n");
}



int sendLol(){
    /*Check for login*/
}
int broadcast(){
    /*Check for login*/
}
int block(){
    /*Check for login*/
}
int unblock(){
    /*Check for login*/
}
int logout(){
    /*Check for login*/
}
int exit_cmd(){
    
}