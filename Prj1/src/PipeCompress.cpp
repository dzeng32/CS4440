/* PipeCompress.. #4 of Project 1
 * Written by: Jason Tejada
 * Desc:
 *      Program that forks two processes; one for reading from a file and the other
 *      for writing to a file. Programs communicate using the pipe system call.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stack>

using namespace std;

int main() {
    string line;

    fstream inFile;
    fstream outFile;
    string source = "public/BigTest.txt";
    string destination = "public/PipeCompressed.txt";

    char newline[1] = {'\n'}, space[1] = {' '};

    pid_t pid;
    int pfd[2];
    
    if(pipe(pfd) == -1) {
        cout << "ERROR: Pipe failed" << endl;
        exit(1); 
    } 

    pid = fork();

    if(pid < 0) {
        cout << "ERROR: Fork failed" << endl; 
    } else if(pid == 0) { // Child
        outFile.open(destination, ios::out);
        close(pfd[1]);     
        char ch[1], prev_bit = 'a';
        int n = 1;
        while(read(pfd[0], ch, 1) > 0) {
            if(ch[0] != '1' && ch[0] != '0' && ch[0] != '\n' && ch[0] != ' ') {
                continue; //Trash
            } else { // Start of line...
                if(prev_bit == 'a') {
                    prev_bit = ch[0]; 
                    continue;
                }
                if(ch[0] == prev_bit) {
                    if(prev_bit == ' ' || prev_bit == '\n') {
                        outFile << prev_bit;
                        continue;
                    }
                    n++;
                } else {
                    if(n < 15) {
                        for(int i = 0; i < n; i++)
                            outFile << prev_bit;  
                    } else {
                        if(prev_bit == '1')
                            outFile << '+' << n << '+'; 
                        else if(prev_bit == '0')
                            outFile << '-' << n << '-'; 
                    }
                    n = 1;
                    prev_bit = ch[0];
                }
            }
        }
        close(pfd[0]);
    } else { // Parent
        inFile.open(source, ios::in);
        close(pfd[0]);
        while(getline(inFile, line)) {
            istringstream iss(line);
            if(line.find(' ') != string::npos) {
                while(getline(iss, line, ' ')) {
                    write(pfd[1], line.c_str(), line.size() + 1); 
                    write(pfd[1], space, 1);
                }            
                write(pfd[1], newline, 1);
            } else {
                write(pfd[1], line.c_str(), line.size() + 1); 
                write(pfd[1], newline, 1);
            }
        }
        close(pfd[1]);
    }

    inFile.close();
    outFile.close();
}
