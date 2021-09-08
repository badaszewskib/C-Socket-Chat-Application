/*Chat client header file. Author: bmbadasz*/

void run_command(char **input);
char **split_line(char *line);
void shell_loop(bool server, int portNo);

