/* ParFork.. #5 of Project 1
 * Written by: Gustavo Torres
 * Desc:
 *      This program compresses a file line in #1, but does it with concurrency.
 *      Divides the file, assemble the result of each process and output..
 */
#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

int chunks = 7;

void processString(string bits, fstream &outFile) {
    char data[bits.size() + 1];
    strcpy(data, bits.c_str());
    int counter = 1;
    string line;
    for(int i = 0; i < bits.size(); i++) {
        if((*(data + i) - *(data + i + 1)) == 0)
            counter++;
        else {
            if(counter < 16) {
                while(counter > 0) {
                    string s(1, data[i]);
                    line = to_string(data[i]);
                    outFile << data[i];
                    counter--;
                }
            }
            else {
                if(data[i] == '1') {
                    line = "+" + to_string(counter) + "+";
                    outFile << line;
                }
                else {
                    line = "-" + to_string(counter) + "-";
                    outFile << line;
                }
            }
            counter = 1;
        }
    }
}

int numberOfLines(fstream& inFile) {
    string line;
    int lineCount = 0;

    while (getline(inFile, line)) {
        lineCount++;
    }
    inFile.close();
    return lineCount;
}

//Divides file into chunks and stores them into an array of vectors
void divideFile(fstream& inFile, vector<string> parts[], int numOfLines, int index) {
    string line;
    int lineCount = 0;
    
    //No remainder
    if (numOfLines % chunks == 0) {
        int linesPerChunk = numOfLines / chunks;
        while (getline(inFile, line)) {
            if (lineCount == linesPerChunk) {
                //cout << "\n";
                index+=1;
                lineCount = 0;
            }
            //cout << line << "\n";
            parts[index].push_back(line);
            lineCount++;
        }
    }
    //If there are remainders when dividing
    else {
        int remainder = numOfLines % chunks;
        int linesPerChunk = numOfLines / chunks;
        int i = 0;
        while (getline(inFile, line)) {
            if (lineCount == linesPerChunk) {
                if (i < remainder) {
                    parts[index].push_back(line);
                    getline(inFile, line);
                    i++;
                }
                index+=1;
                lineCount = 0;
            }
            parts[index].push_back(line);
            lineCount++;
        }
    }
    inFile.close();
}


void iterateThroughVector(vector<string>& parts, fstream &outFile) {
    string str;
    string token;
    for (int i = 0; i < parts.size(); i++){
        str = parts.at(i);
        istringstream iss(str);
        if (str.find(' ') != string::npos) {
            while (getline(iss, token, ' ')) {
                processString(token, outFile);
                outFile << " ";
            }
            outFile << "\n";
            continue;
        }
        processString(str, outFile);
        outFile << "\n";
    }
}

int main() {
    auto start = std::chrono::steady_clock::now();
    vector<string> parts[chunks];
    pid_t pids[chunks];
    
    fstream inFile;
    fstream outFile;
    fstream newFile;
    inFile.open("public/BigTest.txt", ios::in);
    newFile.open("public/BigTest.txt", ios::in);
    outFile.open("public/ParFork_Compressed.txt", ios::out);
    

    //Dividing file into chunks and getting total number of lines in the file
    for (int i = 0; i < chunks; i++) {
        divideFile(newFile, parts, numberOfLines(inFile), i);
    }


    //Creating multiple processes
    for (int i = 0; i < chunks; i++) {
        if ((pids[i] = fork()) < 0) {

        }
        else if (pids[i] == 0) {
            cout << "Reading from file\n";
            iterateThroughVector(parts[i], outFile);
            outFile.close();
            exit(0); 
        }
    }

    //Main process waits for children to finish
    for (int j = 0; j < chunks; j++) {
        wait(NULL);
    }
    auto end = std::chrono::steady_clock::now();
    cout << "Finished Writing to Output File\n";
    double elapsedTimeNs = double(std::chrono::duration_cast <std::chrono::nanoseconds> (end - start).count());
    cout << "----------------------- Time taken: " << elapsedTimeNs / 1e9 << " -----------------------\n";
    return 0;
}
