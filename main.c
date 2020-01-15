#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CMDLINE_MAX 512

int main(void)
{
    char cmd[CMDLINE_MAX];
    char path[1000]; // could be any long
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        while (1)
        {
            char *nl;
            int retval;

            /* Print prompt */
            printf("sshell$ ");
            fflush(stdout);

            /* Get command line */
            fgets(cmd, CMDLINE_MAX, stdin);
            getcwd(path, sizeof(path));
            printf("Current working dir: %s\n", path);
            int i = 0;
            char *token = strtok(cmd, "<|");
            char *commands[16];
            while (token != NULL)
            {
                commands[i] = token;
                i++;
                token = strtok(NULL, "<|");
            }

            if (strstr(commands[0], "cd") != NULL)
            {

                char *token2 = strtok(commands[0], " ");
                char *values[32];
                int j = 0;
                while (token2 != NULL)
                {
                    values[j] = token2;
                    j++;
                    token2 = strtok(NULL, " ");
                }
                getcwd(path, sizeof(path));
                strcat(path, "/");
                strcat(path, values[1]);
                chdir(path);
                printf("Current working dir: %s\n", path);
                getcwd(path, sizeof(path));
                printf("Current working dir: %s\n", path);
            }

            else
            {
                execvp(commands[0], commands);
            }

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
    }

    // hiiii

    return EXIT_SUCCESS;
}
