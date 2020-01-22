#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#define CMDLINE_MAX 512
void run(char *cmd)
{
    int i = 0;
    char *token = strtok(cmd, " ");
    char *arg[10];
    while (token != NULL)
    {
        arg[i] = token;
        i++;
        token = strtok(NULL, " ");
        if (token == NULL)
        {
            arg[i] = token;
        }
    }
    execvp(arg[0], arg);
}
void pipeline(char *process1, char *process2)
{
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0)
    { // child
        close(fd[1]);
        close(STDIN_FILENO);
        dup(fd[0]);
        close(fd[0]);
        run(process2);
    }
    else
    { // parent
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        run(process1);
    }
}
void writeToFile(char *fileName)
{
    int filedesc = open(fileName, O_RDWR);
    dup2(filedesc, STDOUT_FILENO);
    close(filedesc);
}
void redirect(char *process1, char *filename)
{
    writeToFile(filename);
    run(process1);
}

void execute(char *commands[16])
{
    char path[1000]; // could be any long
    pid_t pid;
    if (strstr(commands[0], "cd") != NULL) /// working just need to add
    {
        char *token2 = strtok(commands[0], " ");
        char *values[32];
        int j = 0;
        while (token2 != NULL)
        {
            values[j] = token2;
            j++;
            token2 = strtok(NULL, "");
        }
        getcwd(path, sizeof(path));
        strcat(path, "/");
        strcat(path, values[1]);
        path[strlen(path) - 2] = 0;
        chdir(path); /// needs a space at the end to word e.g "folder "
    }
    else if (strstr(commands[0], "pwd") != NULL)
    {
        getcwd(path, sizeof(path));
        printf("Current working dir: %s\n", path);
    }
    else
    {
        pid = fork();
        if (pid == 0)
        {
            redirect(commands[0], "test.txt");
            //pipeline(commands[0], commands[1]);
            //run(commands[0]);
        }
    }
}
void parse(char *cmd)
{
    int i = 0;
    bool numRedirectError = false;
    bool numPipeError = false;
    if (strstr(cmd, ">&") != NULL)
    {
        numRedirectError = true;
    }

    if (strstr(cmd, "|&") != NULL)
    {
        numPipeError = false;
    }

    char *token = strtok(cmd, ">|");
    char *commands[16];
    while (token != NULL)
    {
        commands[i] = token;
        commands[i][strlen(commands[i]) - 1] = 0; /// adds NULL to each
        i++;
        token = strtok(NULL, ">|");
    }
    execute(commands);
}

void readExecute()
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
        parse(cmd);
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
        // fprintf(stdout, "Return status value for '%s': %d\n",
        //         cmd, retval);
    }
}
int main(void)
{
    readExecute();
    return EXIT_SUCCESS;
}
