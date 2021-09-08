/**
 * @bmbadasz_assignment1
 * @author  Benjamin Badaszewski <bmbadasz@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/global.h"
#include "../include/logger.h"

#include "../include/shell.h"
#include "../include/client.h"
#include "../include/server.h"


/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));

	/*Start Here*/
	int callError = 0;

	/*Check for proper number of arguments*/
	if(argc != 3){
		printf("Program call must have 2 arguments\n");
		callError++;
	}

	/*Check if first argument is either s or c*/
	if(argc>1 && !(*argv[1]=='s' || *argv[1]=='c')){
		printf("First argument must be either s for server mode or c for client mode\n");
		callError++;
	}

	/*Check if the port number is valid*/
	if(argc>2){
		int portNo = atoi(argv[2]);
		if(!(portNo>=1024 && portNo<=65535)){
			printf("Second argument must be a port number between 1024 and 65535\n");
			callError++;
		}
	}

	/*Shut it down if there are any errors in calling*/
	if(callError){
		return callError;
	}

	/*Any call past this point should be valid*/
	int portNum = atoi(argv[2]);
	bool server = false;
	if(*argv[1]=='s'){
		server = true; //Set server mode
	}

	if(server){
		/*Initialize server logic*/
		init_server(portNum);
		// serverFunct(portNum);
	} else {
		/*Initialize client logic*/
		init_client(portNum);
	}

	shell_loop(server, portNum);
	return 0;
}
