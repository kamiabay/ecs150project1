#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command.h"

#define CMDLINE_MAX 512
#define ARG_MAX 16
#define TOKEN_MAX 32

int main(void)
{
        char cmd[CMDLINE_MAX];
        
        while (1) {
                char *nl;

                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);

                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) {
                        printf("%s", cmd);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                nl = strchr(cmd, '\n');
                if (nl)
                        *nl = '\0';

                /* Builtin command */
                if (!strcmp(cmd, "exit")) {
                        fprintf(stderr, "Bye...\n");
                        break;
                }
        	
                /* To parse through `cmd` char by char */
		char cmdInput[TOKEN_MAX + 1];
                for (unsigned int i = 0; i < strlen(cmd); i++) {
                        // Just to see if chars can be added one by one into another string
			cmdInput[i] = cmd[i];

                        /*if (cmd[i] == '|') {

			} else if (cmd[i] == '>') {
			
			} else { continue; } */
		}
                
                // Initializing the object `command`
                command *newCmd = malloc(sizeof(command));

                // This is the `assign()` function from the 
                assign(newCmd, cmdInput);
                // int arg_size = argSize(newCmd); // fix this later
                printf("command is: %s, %s \n", newCmd->SysCall, newCmd->Arguments);
               
                //////////// FORK+EXEC+WAIT PROCESS (NOT DONE) ///////////////
                /* 
                pid_t pid;
                pid = fork();
                int status;

                // i think there is where the cases will have to go

                if (pid == 0){
                        // child stuff happens here
                        printf("Went into child process!");
                        execvp(newCmd->SysCall, newCmd->Arguments);
                        
                } else if (pid > 0) {
                        // parent stuff happens here
                        waitpid(-1, &status, 0);
                        fprintf(stderr, "+ completed '%s' [%i] \n", cmd, WEXITSTATUS(status));
                } else {
                        perror("fork");
                        exit(1);
                } */
                
        }

        return EXIT_SUCCESS;
}
