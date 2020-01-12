#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CMDLINE_MAX 512

int main(void)
{
    char cmd[CMDLINE_MAX];

    while (1)
    {
        char *nl;
        int retval;

        /* Print prompt */
        printf("sshell$ ");
        fflush(stdout);

        /* Get command line */
        fgets(cmd, CMDLINE_MAX, stdin);

        int i = 0;
        char *token = strtok(cmd, "<|");
        char *commands[16];
        char *val;
        while (token != NULL)
        {
            commands[i] = token;
            i++;
            token = strtok(NULL, " ");
            //strcat(val, token);
        }
        printf("%s", commands);
        execvp(commands[0], commands);
        // execvp(cmd[0], cmd);
        // char *s[4] = {"ls", "-l", "-a", NULL};
        // execvp(s[0], s);
        /* Print command line if stdin is not provided by terminal */
        if (!isatty(STDIN_FILENO))
        {
            printf("%s", cmd);
            fflush(stdout);
        }

        /* Remove trailing newline from command line */
        nl = strchr(cmd, '\n');
        if (nl)
            *nl = '\0';

        /* Builtin command */
        if (!strcmp(cmd, "exit"))
        {
            fprintf(stderr, "Bye...\n");
            break;
        }

        /* Regular command */
        retval = system(cmd);
        fprintf(stdout, "Return status value for '%s': %d\n",
                cmd, retval);
    }

    // hiiii

    return EXIT_SUCCESS;
}
