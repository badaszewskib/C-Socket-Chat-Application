#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/queue.h>

/*Chat server header file. Author: bmbadasz*/

void init_server(int portNum);
int statistics();
int blocked();

struct entry {
    char hostname[35];
    char ip[INET_ADDRSTRLEN];
    int port;
    bool loggedIn;
    SLIST_ENTRY(entry) entries;
};

SLIST_HEAD(slisthead, entry);
struct slisthead head;
void serverDataInsert(struct entry * newEnt);