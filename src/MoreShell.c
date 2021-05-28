/* MoreShell.. #7 for Project 1
 * Written By: Gustavo Torres
 * Desc:
 * 	Makes the minishell from #6 more powerful by allowing args to
 *      the commands.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    char command[50];
    printf("<MoreShell>\n");
    fgets(command, 50, stdin);
    

    while (strncmp(command, "exit", 4) != 0) {
        char *args[4];
        //Set new line character to null
        command[strcspn(command, "\n")] = '\0';
        char *tokens = strtok(command, " ");
        int count = 0;
        while (tokens != NULL){
            args[count] = (char*) malloc(5 * sizeof(char));
            strcpy(args[count], tokens);
            count++;
            tokens = strtok(NULL, " ");
        }
        
        args[count] = NULL;
        pid_t pid;
        pid = fork(); //Create new Process;

        if (pid < 0) {
            printf("ERROR\n");
        }

        else if (pid == 0) {
            //Pass command and arguements to execvp
            printf("Running program...\n");
            execvp(args[0], args);
        }

        else {
            wait(&pid);
            printf("Finished\n");
        }
        
        //Set string to null
        command[0] = '\0';

        printf("<MoreShell>\n");
        fgets(command, 50, stdin);
    }
}
