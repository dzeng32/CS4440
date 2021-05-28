/* MyCompress.. #1 of Project 1
 * Written by: Jason Tejada
 * Desc:
 *      This program makes a compressed copy of an existing file of bits using system
 *      calls for file manipulation. The rules of compression are on the project 1 
 *      document... if there is a sequence of 1s and 0s greater than or equal to 16,
 *      compress.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>

using namespace std;

// Compression happens here
void processString(string bits, fstream& outFile) {
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
                    counter--;
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

int main(int argc, char *argv[]) {

    // This program must accept args.. eg: ./MyCompress public/BigTest.txt public/MyCompress_Compressed.txt
    if(argc != 3) {
        cout << "INVALID ARGS: ./MyCompress SOURCE DESTINATION\n"; 
        return 0;
    }

    fstream inFile;
    fstream outFile;
    string str;
    string token;
    
    string source = argv[1];
    string destination = argv[2];

    inFile.open(source, ios::in);
    outFile.open(destination, ios::out);

    // Reads file, sends strings to get compressed and written to destination.
    while(getline(inFile, str)) {
        istringstream iss(str);
        if(str.find(' ') != string::npos) {
            while(getline(iss, token, ' ')) {
                processString(token, outFile);
                outFile << " ";
            }
            outFile << "\n";
            continue;
        }
        processString(str, outFile);
        outFile << "\n";
    }
    inFile.close();
    outFile.close();
}
