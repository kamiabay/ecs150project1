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
#define MAX_SIZE_STRING 16
///////
//////
////
///
// our porgram works perfect on mac/csif yet we use tster or gardescp it looks like it is not reading our output

void printError(char *errorMessage) // prints the erros
{
    fprintf(stderr, "%s", errorMessage);
    //  fflush(stderr);
    // exit(-1);
}
char *removeWhiteSpace(char *string) // removes white spaces in a string
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
void run(char *cmd) // a function that should work like system just gets the string and does everything and calls execvp
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
        //exit(EXIT_FAILURE);
    }
}

void pipeline(char *process1, char *process2)
{
    int fd[2];
    pipe(fd); // makes a little buffer area i.e the pipe
    pid_t pid = fork();
    if (pid == 0)
    {                               // child
        dup2(fd[1], STDOUT_FILENO); /// makes it so now it will write to the pipe
        close(fd[1]);
        run(process1); // run the process // out put goes into file
    }
    else
    { // parent
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO); // points what comes in to the pipe
        close(fd[0]);
        run(process2); // runs what was in the pipe
    }
}
void writeToFile(char *fileName, int typeOfFile)
{
    fileName = removeWhiteSpace(fileName); /// removes white space
    int filedesc = open(fileName, O_RDWR | O_CREAT);
    if (filedesc < 0)
        printError("Error: cannot open output file\n");
    if (typeOfFile == STDERR_FILENO) // if its a type >& then send both error and output to the file
    {
        dup2(filedesc, STDOUT_FILENO);
        dup2(filedesc, STDERR_FILENO);
    }
    else
        dup2(filedesc, STDOUT_FILENO); // only iutput goes to the file
    close(filedesc);
}

int is_empty(const char *s)
{                              // easy to implement, literally had no time
    while (isspace(*s) && s++) //this is going thorugh the string and
        ;                      // if it sees anything but a space it returns with false
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
        writeToFile(filename, typeOfFile); // points the output to the file depending if > or >&
        run(process1);                     // runs the process and everything goes to the file
    }
}

void execute(char *originalCommand, char *commands[16], char *type)
{

    char path[1000];                       // could be any long
    if (strstr(commands[0], "cd") != NULL) /// does the cd function
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
        if (existValue != 0)
            fprintf(stderr, "Error: no such directory\n");

        fprintf(stderr, "+ completed '%s %s' [%i]\n", commands[0], values[1], existValue);
    }
    else if (strstr(commands[0], "pwd") != NULL) // does pwd function
    {
        getcwd(path, sizeof(path));
        // fflush(stdout);
        fprintf(stdout, "%s\n", path);
        fprintf(stderr, "+ completed '%s' [0]\n", commands[0]); // works
    }
    else
    {
        int status;
        pid_t pid;
        pid = fork();
        if (pid == 0) // run the program in a child environment and come back to the parnet after
        {
            if (!strcmp(type, "redirect"))
                redirect(commands[0], commands[1], STDOUT_FILENO);
            else if (!strcmp(type, "redirectError"))
                redirect(commands[0], commands[1], STDERR_FILENO);
            else if (!strcmp(type, "pipe"))
                pipeline(commands[0], commands[1]);
            else
                run(commands[0]);
            perror("problem");
            exit(1);
        }
        else if (pid > 0)
        {
            waitpid(-1, &status, 0);
            fprintf(stderr, "+ completed '%s' [%d]\n", originalCommand, WEXITSTATUS(status));
        }
        else
        {
            perror("fork");
            exit(1);
        }
    }
}
void parse(char *cmd)
{
    char *originalCommand = strdup(cmd);                 // duplicates the originals commands and daves it
    originalCommand[strlen(originalCommand) - 1] = '\0'; // makes it null terminated
    bool isRedirect = false, isPipe = false, isRedirectError = false, isPipeError = false;
    /// checks to see what is in the command so it can later call the correct execute fucntion
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
    char *commands[MAX_SIZE_STRING];
    while (token != NULL)
    {
        //token[strlen(token) - 1] = '\0';
        commands[i] = token;
        commands[i][strlen(commands[i]) - 1] = '\0'; /// adds NULL to each
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
        if (strstr(cmd, "exit") != NULL)
        {
            fprintf(stderr, "Bye...\n");
            fprintf(stderr, "+ completed 'exit' [0]\n");
            break;
        }
        else
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
    }
}
int main(void)
{
    readExecute();
    return EXIT_SUCCESS;
}
