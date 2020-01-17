#define COMMAND_H
#include <stdio.h>
#include <string.h>

#define ARG_MAX 16

typedef struct Command {
    char *SysCall;
    char *Arguments; // do i make this an array of arguments?
} command;

void assign(command *newCmd, char *cmdInput) {
	int size = strlen(cmdInput); int j = 0;      
    char *string = malloc(sizeof(char)*sizeof(size));

    //printf("About to enter for loop!\n");
    for (int i = 0; i < size; i++) {
        //printf("cmdInput[i] before if statement: %c\n", cmdInput[i]);
        if (cmdInput[i] == ' ') {
            //printf("Entered space if! \n");
            if (newCmd->SysCall == NULL) {
                //printf("Entered SysCall if! This is string: %s \n", string);
                newCmd->SysCall = string;
                j = 0;
            } else { 
                string[j] = cmdInput[i];
                //printf("string[%d] = %c; cmdInput[%d] = %c\n", j, string[j], i, cmdInput[i]);
                j++;
            }
        } else if (i == size - 1) {
            string[j] = cmdInput[i];
            //printf("string[%d] = %c; cmdInput[%d] = %c\n", j, string[j], i, cmdInput[i]);
            //printf("Entered else if! This is string for arg: %s \n", string);
            newCmd->Arguments = string;
            break;
        } else {
            string[j] = cmdInput[i];
            //printf("string[%d] = %c; cmdInput[%d] = %c\n", j, string[j], i, cmdInput[i]);
            j++;
        }
    }
}

