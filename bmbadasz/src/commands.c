#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "../include/logger.h"
#include "../include/server.h"

/*Author function, prints recognition of course academic integrity policy*/
int author(){
    char *command_str = "AUTHOR";
    char *your_ubit_name = "bmbadasz";
    cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", your_ubit_name);
    cse4589_print_and_log("[%s:END]\n", command_str);
}

/**
 * Get your own IP
 * @param output_str Where you want the output written to
 * @param portNum Port you are listening on
*/
void get_own_ip(char* output_str, int portNum){
    /*Convert portNum to a string*/
    char portStr[8];
    sprintf(portStr, "%d", portNum);
    /*Basic idea, get host name and feed that into getaddrinfo*/
    char hostname[128];
    if(gethostname(hostname, sizeof(hostname)) == -1){
        return; //gethostname fails
    }
    struct addrinfo hints;
    struct addrinfo *results;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int status;
    if((status = getaddrinfo(hostname, portStr, &hints, &results)) != 0){
        return;
    } else {
        struct sockaddr_in *result_addr = (struct sockaddr_in *)results->ai_addr;
        char retVal[INET_ADDRSTRLEN];
        void *addr;
        addr = &(result_addr->sin_addr);
        inet_ntop(results->ai_family, addr, retVal, sizeof(retVal));
        sprintf(output_str, "%s", retVal);
        freeaddrinfo(results);
    }
}

/*Ip function, prints external IP address of the process*/
int ip(int portNum){
    char *command_str = "IP";
    /*You have to call get_own_ip like this because blah*/
    char ipstr[INET_ADDRSTRLEN];
    memset(ipstr, '\0', sizeof(ipstr));
    get_own_ip(ipstr, portNum);
      
    if(strlen(ipstr) == 0){
        cse4589_print_and_log("[%s:ERROR]\n", command_str);
    } else {
        cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
        cse4589_print_and_log("IP:%s\n", ipstr);
    }
    cse4589_print_and_log("[%s:END]\n", command_str);
}



/*Port function, prints the port the process is listening on*/
int port(int portNum){
    char *command_str = "PORT";
    cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
    cse4589_print_and_log("PORT:%d\n", portNum);
    cse4589_print_and_log("[%s:END]\n", command_str);
}

int list(){
    char *command_str = "LIST";
    struct entry *trav;
    if(SLIST_EMPTY(&head)){
        cse4589_print_and_log("[%s:ERROR]\n", command_str);
        cse4589_print_and_log("[%s:END]\n", command_str);
        return -1;
    }
    cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
    int listID = 1;
    SLIST_FOREACH(trav, &head, entries){
        cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", listID, trav->hostname, trav->ip, trav->port);
        listID++;
    }
    cse4589_print_and_log("[%s:END]\n", command_str);
}