#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/queue.h>

#include "../include/commands.h"
#include "../include/server.h"

void sendServerData(int socket){
    struct entry *trav;
    char buffer[1024];
    memset(buffer, '\0', strlen(buffer));
    SLIST_FOREACH(trav, &head, entries){
        char infostr[512];
        memset(infostr, '\0', strlen(infostr));
        sprintf(infostr, "%s;%s;%d;%s;\n", trav->hostname, trav->ip, trav->port, trav->loggedIn?"true":"false");
        strcat(buffer, infostr);
    }
    send(socket, buffer, strlen(buffer), 0);
}

void serverDataInsert(struct entry * newEnt){
    struct entry *trav;
    trav = SLIST_FIRST(&head);
    if(trav == NULL || (trav->port >= newEnt->port)){
        SLIST_INSERT_HEAD(&head, newEnt, entries);
    } else {
        struct entry *temp = SLIST_NEXT(trav, entries);
        while(temp != NULL && (temp->port < newEnt->port)){
            trav = temp;
            temp = SLIST_NEXT(trav, entries);
        }
        SLIST_INSERT_AFTER(trav, newEnt, entries);
    }
}

void handleLogin(int socket, char **input){
    /*Assume input is cmd;hostname;ip;port*/
    struct entry *newEnt;
    newEnt = malloc(sizeof(struct entry));
    strcpy(newEnt->hostname, input[1]);
    strcpy(newEnt->ip, input[2]);
    int porttmp = atoi(input[3]);
    newEnt->port = porttmp;
    newEnt->loggedIn = true;
    serverDataInsert(newEnt);
    sendServerData(socket);
}

void handleLogout(int socket){
    
}

#define TOKEN_BUFFER_SIZE 64
#define PACKET_DELIMITERS ";"
char **splitIncoming(char *packet){
    int token_buffer_size = TOKEN_BUFFER_SIZE;
	int trav = 0;
	char **tokens = malloc(token_buffer_size * sizeof(char*));
	char *curr;
	char *token = strtok(packet, PACKET_DELIMITERS);
	while(token != NULL) {
		tokens[trav] = token;
		trav = trav + 1;
		if(trav >= token_buffer_size){
			token_buffer_size += TOKEN_BUFFER_SIZE;
			tokens = realloc(tokens, token_buffer_size * sizeof(char*));
		}
	token = strtok(NULL, PACKET_DELIMITERS);
	}
	tokens[trav] = NULL;
	return tokens;
}

void parseIncoming(int socket, char *packet){
    char **input = splitIncoming(packet);
    if(input[0]!=NULL){
        if(strcmp(input[0], "LOGIN") == 0){
            handleLogin(socket, input);
        }
        if(strcmp(input[0], "REFRESH") == 0){
            sendServerData(socket);
        }
        if(strcmp(input[0], "LOGOUT") == 0){
            handleLogout(socket);
        }
    }

}

void init_server(int portNum){
    /*Initialize client list*/
    SLIST_INIT(&head);

    /*Set up the different variables needed*/
    struct sockaddr_in server;
    int listener;
    int newfd;
    struct sockaddr_storage remote;
    socklen_t ad_size;
    char buffer[1024];  
    fd_set master; //fd list
    fd_set read_fds; //temp fd list
    int fdmax;  
    int yes = 1;
    int i, j, rv;
    int nbytes;
    socklen_t addrlen;

    /*Set up the server socket*/
    listener = socket(PF_INET, SOCK_STREAM, 0);
    memset(&server, '\0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(portNum);
    /*You have to call get_own_ip like this because blah*/
    char ipstr[INET_ADDRSTRLEN];
    memset(ipstr, '\0', sizeof(ipstr));
    get_own_ip(ipstr, portNum);
    server.sin_addr.s_addr=inet_addr(ipstr);

    /*Set up stuff for select function*/
    FD_ZERO(&master);
    FD_ZERO(&read_fds);


    bind(listener, (struct sockaddr*)&server, sizeof(server));
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    listen(listener, 10);
    
    FD_SET(listener, &master);
    fdmax = listener;
    
    for(;;){
        read_fds = master;
        if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
            printf("Select error\n");
            return;
        }

        for(i=0; i<=fdmax; i++){
            if(FD_ISSET(i, &read_fds)){
                if(i==listener){
                    addrlen = sizeof(remote);
                    newfd = accept(listener, (struct sockaddr *)&remote, &addrlen);

                    if(newfd == -1){
                        printf("Accept error\n");
                    } else {
                        FD_SET(newfd, &master);
                        if(newfd > fdmax) {
                            fdmax = newfd;
                        }
                    }
                } else {
                    /*Handle incoming info*/
                    nbytes = read(i, buffer, sizeof(buffer));
                    parseIncoming(i, buffer);
                    memset(buffer, '\0', sizeof(buffer));            
                }
            }
        }
    }
}

int statistics(){

}
int blocked(){
	
}
