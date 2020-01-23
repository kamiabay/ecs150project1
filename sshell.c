#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>

#define CMDLINE_MAX 512
#define ARG_MAX 16

void printError(char *errorMessage)
{
    fprintf(stderr, "%s", errorMessage);
    fflush(stderr);
    exit(-1);
}
char *removeWhiteSpace(char *string)
{
    int i = 0;
    char *token = strtok(string, " ");
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
    return arg[0];
}
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
    if (i > ARG_MAX)
    {
        printError("Error: too many process arguments\n");
    }
    else
    {
        execvp(arg[0], arg);
    }
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
    int filedesc = open(fileName, O_RDWR | O_CREAT);
    //perror("open");
    if (filedesc < 0)
    {
        printError("Error: cannot open output file\n");
    }
    dup2(filedesc, STDOUT_FILENO);
    close(filedesc);
}

int is_empty(const char *s)
{
    while (isspace(*s) && s++)
        ;
    return !*s;
} //// https://stackoverflow.com/questions/3981510/getline-check-if-line-is-whitespace

void redirect(char *process1, char *filename)
{
    if (strlen(process1) == 0)
    {
        printError("Error: missing command\n");
    }
    else if (is_empty(filename))
    {
        printError("Error: no output file\n");
    }
    else
    {
        writeToFile(filename);
        run(process1);
    }
}

void execute(char *commands[16], char *type)
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
        chdir(path);
    }
    else if (strstr(commands[0], "pwd") != NULL)
    {
        getcwd(path, sizeof(path));
        fflush(stdout);
        printf("%s\n", path);
    }
    else
    {
        pid = fork();
        if (pid == 0)
        {
            char *rdr = "redirect";
            char *pipe = "pipe";
            if (!strcmp(type, rdr))
            {
                redirect(commands[0], commands[1]);
            }
            else if (!strcmp(type, pipe))
            {
                pipeline(commands[0], commands[1]);
            }
            else
                run(commands[0]);
        }
        int status;
        wait(&status);
        printf("after child\n");
    }
}
void parse(char *cmd)
{

    bool isRedirect = false, isPipe = false;
    if (strstr(cmd, ">") != NULL)
        isRedirect = true;

    if (strstr(cmd, "|") != NULL)
        isPipe = true;

    int i = 0;
    char *token = strtok(cmd, ">|");
    char *commands[16];
    while (token != NULL)
    {
        commands[i] = token;
        //printf("command is =%s\n", commands[i]);
        commands[i][strlen(commands[i]) - 1] = 0; /// adds NULL to each
        i++;
        token = strtok(NULL, ">|");
    }
    if (isRedirect)
        execute(commands, "redirect");
    if (isPipe)
        execute(commands, "pipe");
    else
        execute(commands, "none");
}

void readExecute()
{
    char cmd[CMDLINE_MAX];
    while (1)
    {
        char *nl;
        // int retval;
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
