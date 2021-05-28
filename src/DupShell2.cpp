/* DupShell.. #8
 * Written by: Daniel Zeng
 * Desc:
 *      Added to the MoreShell capability of exeucuting command lines
 *      with commands connected by pipes. Uses dup2 system call.
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
    int pipefds[2];
    pid_t pid1;
    pid_t pid2;
    
    pipe(pipefds);

    while (strcmp(input, "exit") != 0) {

        cout << "DupShell> ";
        cin.getline(input,50);

        //char **command = (char**) malloc(8 * sizeof(char*));
        char* command[20];
        int index =0;
        char *parsed;

        /* Parsing user input */
        parsed = strtok(input, " ");
        while(parsed && strcmp(parsed, "|") != 0){
        // while(parsed != NULL){
            command[index] = parsed;
            //cout << "Before index++: " << command[index] << endl;
            index++;
            
            parsed = strtok(NULL, " ");
            //cout << "After index++ and NULL: " << command[index] << endl;
            
        }

        command[index] = NULL;

        pid1 = fork();

        if(pid1 < 0){
            perror("ERROR: fork failed\n");
            exit(1);
        }
        if (pid1 == 0) {

            dup2(pipefds[1], STDOUT_FILENO);     //replace stdout with the write end of the pipe
            close(pipefds[0]);                   //close read to pipe, in child
            
            /* Never returns if the call is successful */
            execvp(command[0], command);        //this command probably needs to be changed for dup
            printf("This won't be printed if execvp is successful\n");

        } else {
            pid2 = fork();
            if(pid2 < 0){
                perror("ERROR: fork failed\n");
                exit(1);
            }

            if(pid2 == 0){

                dup2(pipefds[0], STDIN_FILENO);  //replace stdin with the read end of the pipe
                close(pipefds[1]);              //close write to pipe, in parent

                execvp(command[0], command);    //this command probably needs to be changed for dup
                printf("This won't be printed if execvp isS successful\n");

            }
            else{

                close(pipefds[0]);
                close(pipefds[1]);
                //wait for both childs
                wait(NULL);
                wait(NULL);

                
            }
        }

    }

    return 0;
}
