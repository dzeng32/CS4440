/* MiniShell.. #6 of Project 1
 * Written by: Daniel Zeng
 * Desc: A shell like program that illustrates how UNIX spawns processes.
 *       Simple program that prompt to the user, read the command from the
 *       input and execute the command.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <iostream>
#include <string.h>

using namespace std;

int main() {

    char *input;

    pid_t pid;
    int stat_loc;

    while (strcmp(input, "exit") != 0) {

        cout << "MiniShell> ";
        cin.getline(input,50);

        char **command = (char**) malloc(8 * sizeof(char*));
        int index =0;
        char *parsed;

        parsed = strtok(input, " ");
        while(parsed != NULL){
            command[index] = parsed;
            index++;
            
            parsed = strtok(NULL, " ");
        }

        command[index] = NULL;

        pid = fork();

        if(pid < 0){
            perror("ERROR: fork failed\n");
            exit(1);
        }
        if (pid == 0) {
            /* Never returns if the call is successful */
            execvp(command[0], command);
            printf("This won't be printed if execvp is successul\n");
        } else {
            wait(NULL);
        }

    }

    return 0;
}
