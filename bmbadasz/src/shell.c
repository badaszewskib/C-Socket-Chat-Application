#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/commands.h"
#include "../include/server.h"
#include "../include/client.h"

#define TOKEN_BUFFER_SIZE 64
#define TOKEN_DELIMITERS " \t\r\n\a"

/*If true then is a server, false is a client*/
bool srvMode;
int portNum;

/*List of universal commands*/
char *commands[] = {
	"AUTHOR",
	"IP",
	"PORT",
	"LIST"
};
/*Pointers to command functions*/
int (*command_functions[]) (int, char **) = {
	&author,
	&ip,
	&port,
	&list
};

/*List of server commands*/
char *srvCommands[] = {
	"STATISTICS",
	"BLOCKED"
};
/*Pointers to server command functions*/
int (*srvCommand_functions[]) (int, char **) = {
	&statistics,
	&blocked
};

/*List of client commands*/
char *cliCommands[] = {
	"LOGIN",
	"REFRESH",
	"SEND",
	"BROADCAST",
	"BLOCK",
	"UNBLOCK",
	"LOGOUT",
	"EXIT"
};
/*Pointers to client command functions*/
int (*cliCommand_functions[]) (int, char **) = {
	&login,
	&refresh,
	&sendLol,
	&broadcast,
	&block,
	&unblock,
	&logout,
	&exit_cmd
};



/*Interpret tokens and run appropriate command*/
int run_command(char **input){
	/*Check for an empty input*/
	if(input[0] == NULL){
		return -1;
	}

	/*Check for an ordinary command*/
	for(int i=0; i<(sizeof(commands)/sizeof(char *)); i++){
		if(strcmp(input[0], commands[i]) == 0){
			return (*command_functions[i]) (portNum, input);
		}
	}

	/*Check for server commands*/
	if(srvMode){
		for(int i=0; i<(sizeof(srvCommands)/sizeof(char *)); i++){
		
		}
	} 
	
	else { /*Check for client commands*/
		for(int i=0; i<(sizeof(cliCommands)/sizeof(char *)); i++){
			if(strcmp(input[0], cliCommands[i]) == 0){
				return (*cliCommand_functions[i]) (portNum, input);
			}
		}
	}
}
/*END RUN_COMMAND*/



/*Tokenize the user input*/
char **split_line(char *line){
	int token_buffer_size = TOKEN_BUFFER_SIZE;
	int trav = 0;
	char **tokens = malloc(token_buffer_size * sizeof(char*));
	char *curr;
	char *token = strtok(line, TOKEN_DELIMITERS);
	while(token != NULL) {
		tokens[trav] = token;
		trav = trav + 1;
		if(trav >= token_buffer_size){
			token_buffer_size += TOKEN_BUFFER_SIZE;
			tokens = realloc(tokens, token_buffer_size * sizeof(char*));
		}
	token = strtok(NULL, TOKEN_DELIMITERS);
	}
	tokens[trav] = NULL;
	return tokens;
}
/*END SPLIT_LINE*/



/*Run the logic for the SHELL functionality*/
int shell_loop(bool server, int portNo){
	srvMode = server;
	portNum = portNo;
	char *line;
    char **input;
	int status = true;
	/*Run the CLI*/
	while(status){
		printf("> ");

		/*Initialize the line of input being read*/
    	line = NULL;
		size_t lineSize = 0;
		if(getline(&line, &lineSize, stdin) == -1){
			printf("Error reading user input\n");
		}
		/*God bless the getline function*/

		/*Take input, split it up so it's usable*/
		input = split_line(line);

		/*Parse for commands and run them*/
		run_command(input);

		/*Clean up*/
		free(line);
		free(input);
	}
}
/*END SHELL_LOOP*/

