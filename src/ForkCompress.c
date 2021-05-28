/* Fork Compress.. #3 of Project 1
 * Written By: Jason Tejada
 * Desc: 
 *      Creates a new process to compress a file using MyCompress. Source & destination file 
 *      names presented as command-line arguments... Main process waits...
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void childProcess(char *source, char *destination) {
    printf("starting compression...\n");
    printf("SOURCE: %s\n", source);
    printf("DESTINATION: %s\n", destination);
    printf("waiting...\n");
    execl("./MyCompress", "./MyCompress", source, destination, NULL);
}

int main() {
    char *source = "public/BigTest.txt";
    char *destination = "public/ForkCompressed.txt";

    pid_t pid;
    pid = fork(); // Creating new process

    if(pid < 0) {
        printf("ERROR\n"); 
    }

    else if(pid == 0)
        childProcess(source, destination);
    else {
        wait(&pid);
        printf("compressed...\n");
        printf("finished compression: wrote to \'%s\' using \'%s\'\n", source, destination);
    }
}
