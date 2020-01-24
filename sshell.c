#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CMDLINE_MAX 512
#define ARG_MAX 16

int EXIT_GLOBAL = 0;
//sdsdsd

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
        printError("Error: too many process arguments\n");
    else
    {
        execvp(arg[0], arg);
        exit(EXIT_FAILURE);
    }
}

void pipeline(char *process1, char *process2)
{
    int fd[2];
    pipe(fd); // makes a little buffer area i.e the pipe
    pid_t pid = fork();
    if (pid == 0)
    { // child
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        run(process1);
    }
    else
    { // parent
        int status;
        wait(&status);
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        run(process2);
    }
}
void writeToFile(char *fileName, int typeOfFile)
{
    fileName = removeWhiteSpace(fileName); /// should work
    int filedesc = open(fileName, O_RDWR | O_CREAT);
    //perror("open");
    if (filedesc < 0)
        printError("Error: cannot open output file\n");
    // printf("value = %i", typeOfFile);
    if (typeOfFile == STDERR_FILENO)
    {
        fflush(stdout);
        fflush(stderr);
        dup2(filedesc, STDOUT_FILENO);
        dup2(filedesc, STDERR_FILENO);
    }
    else
        dup2(filedesc, STDOUT_FILENO);
    close(filedesc);
}

int is_empty(const char *s)
{
    while (isspace(*s) && s++)
        ;
    return !*s;
} //// https://stackoverflow.com/questions/3981510/getline-check-if-line-is-whitespace

void redirect(char *process1, char *filename, int typeOfFile)
{
    if (strlen(process1) == 0)
        printError("Error: missing command\n");
    else if (is_empty(filename))
        printError("Error: no output file\n");
    else
    {
        writeToFile(filename, typeOfFile);
        run(process1);
    }
}

void execute(char *originalCommand, char *commands[16], char *type)
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
        int existValue = chdir(path);
        if (existValue != 0) /// works
            printf("Error: no such directory\n");
        printf("+ completed '%s %s' [%i] \n", commands[0], values[1], existValue);
    }
    else if (strstr(commands[0], "pwd") != NULL)
    {
        getcwd(path, sizeof(path));
        fflush(stdout);
        printf("%s\n", path);
        printf("+ completed '%s' [0] \n", commands[0]); // works
    }
    else
    {
        pid = fork();
        if (pid == 0)
        {
            if (!strcmp(type, "redirect"))
                redirect(commands[0], commands[1], STDOUT_FILENO);
            else if (!strcmp(type, "redirectError"))
                redirect(commands[0], commands[1], STDERR_FILENO);
            else if (!strcmp(type, "pipe"))
                pipeline(commands[0], commands[1]);
            else
                run(commands[0]);
        }
        int status;
        wait(&status);
        if (!strcmp(type, "redirect"))
        {
            printf("+ completed '%s' [%d] \n", originalCommand, WEXITSTATUS(status));
        }
        else
            printf("+ completed '%s' [%d] \n", originalCommand, WEXITSTATUS(status));
    }
}
void parse(char *cmd)
{
    char *originalCommand = strdup(cmd);
    originalCommand[strlen(originalCommand) - 1] = '\0';
    bool isRedirect = false, isPipe = false, isRedirectError = false, isPipeError = false;
    if (strstr(cmd, ">&") != NULL)
        isRedirectError = true;
    else if (strstr(cmd, "|&") != NULL)
        isPipeError = true;
    else if (strstr(cmd, ">") != NULL)
        isRedirect = true;
    else if (strstr(cmd, "|") != NULL)
        isPipe = true;

    int i = 0;
    char *token = strtok(cmd, ">|&");
    char *commands[16];
    while (token != NULL)
    {
        // printf("before = '%s'\n", token);
        // printf("before size = %lu\n", strlen(token));
        //token[strlen(token) - 1] = '\0';
        commands[i] = token;
        commands[i][strlen(commands[i]) - 1] = '\0'; /// adds NULL to each
        // printf("after = '%s'\n", commands[i]);
        // printf("after size = %lu\n", strlen(commands[i]));
        i++;
        token = strtok(NULL, ">|&");
    }

    if (isRedirect)
        execute(originalCommand, commands, "redirect");
    else if (isRedirectError)
        execute(originalCommand, commands, "redirectError");
    else if (isPipe)
        execute(originalCommand, commands, "pipe");
    else if (isPipeError)
        execute(originalCommand, commands, "pipeError");
    else
        execute(originalCommand, commands, "none");
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
        if (strstr(cmd, "exit") != NULL)
        {
            fprintf(stderr, "Bye...\n");
            break;
        }
    }
}
int main(void)
{
    readExecute();
    return EXIT_SUCCESS;
}
