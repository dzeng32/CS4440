/* ParThread.. #9 of Project 1
 * Written by: Jason Tejada
 * Desc:
 *      Uses the pthread library to write a solution similar to #5 of the project.
 *      (Conccurrency, compression)
 */
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <semaphore.h>
#include <time.h>

using namespace std;

// Datafields
const int chunks = 10;
int passes = 0;
vector<string> parts[chunks];

// Synchronization
#define MAX_SIZE 1
sem_t work;

// Compression happens here
void processString(string bits, fstream &outFile) {
    char data[bits.size() + 1];    
    strcpy(data, bits.c_str());

    int counter = 1;
    for(int i = 0; i < bits.size(); i++) {
        if((*(data + i) - *(data + i + 1)) == 0)
            counter++;
        else {
            if(counter < 16) {
                while(counter > 0) {
                    outFile << data[i];
                    counter --; 
                } 
            } 

            else {
                if(data[i] == '1')
                    outFile << "+" << counter << "+";
                else
                    outFile << "-" << counter << "-";
            }

            counter = 1;
        }
    }
}

// Send data to get processes
void *start(void * params) {
    sem_wait(&work); 
    string line;
    fstream* outFile = static_cast<fstream*>(params);
    for(int i = 0; i < parts[passes].size(); i++) {
        line = parts[passes].at(i);
        istringstream iss(line);
        if(line.find(' ') != string::npos) {
            while(getline(iss, line, ' ')) {
                processString(line, *outFile);
                *outFile << " ";
            } 
            *outFile << "\n";
            continue;
        }
        processString(line, *outFile);
        *outFile << endl;
    }
    sem_post(&work); 
    cout << "Worker done: " << passes << endl;

    // How we keep track of which part of the file the thread works on
    if(passes == chunks)
        pthread_exit(NULL);
    passes++;
    pthread_exit(NULL);
}

int main(void) {
    clock_t tStart = clock();
    pthread_t thread_id[chunks];
    int n = 0;

    // Getting number of lines and defining ranges....
    fstream lineCounter("public/BigTest.txt", ios::in);
    string line;
    while(getline(lineCounter, line))
        n++;
    lineCounter.close();

    // Poplating the parts field with data from file
    int lineCount = 0;
    int index = 0;
    fstream reader("public/BigTest.txt", ios::in);
    for(int i = 0; i < chunks; i++) {
        while(getline(reader, line)) {
            if(lineCount >= n / chunks) {
                index += 1;
                lineCount = 0;
            } 

            parts[index].push_back(line);
            lineCount++;
        }
    }
    
    sem_init(&work, 0, MAX_SIZE);

    // Starting compression steps...
    fstream outFile("public/ParThread_Compressed.txt", ios::out);
    for(int i = 0; i < chunks; i++) { // Dispatch work
        pthread_create(&thread_id[i], NULL, &start, static_cast<void*>(&outFile));
    }
    for(int i = 0; i < chunks; i++) // End work
        pthread_join(thread_id[i], NULL);
    cout << "----------------------- Time taken: " << (double) (clock() - tStart) / CLOCKS_PER_SEC << " -----------------------\n";
}
