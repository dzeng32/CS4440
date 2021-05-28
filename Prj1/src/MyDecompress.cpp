/* MyDecompress.. #2 of Project 1
 * Written by: Jason Tejada
 * Desc:
 *      This program decompresses a file. Input is a compressed file,
 *      out put is the file in its original form (decompressed).
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>

using namespace std;

// Get the number of characters between a pair of '+' or '-'.
int getGap(char arr[], int n, char c) {
    int i = 0, j;
    for(i = 0; i < n; i++)
        if(arr[i] == c)
            break;
    if(i >= n - 1)
        return 0;
    for(j = i + 1; j < n; j++) 
        if(arr[j] == c)
            break;
    if(j==i)
        return 0;
    return(j - i - 1);
}

// Decompress the bits, write into destination file
void decompress(string str, char c, fstream& outFile) {
    int n_compressedBits = stoi(str);
    char bit[2];
    
    if(c == '-')
        bit[0] = '0';
    else
        bit[0] = '1';
    for(int i = 0; i < n_compressedBits; i++) {
        outFile << bit[0];
    }

    return;
}

void processString(string bits, fstream& outFile) {
    char data[bits.size() + 1];
    strcpy(data, bits.c_str());

    for(int i = 0; i < bits.size(); i++) {
        if(data[i] == '0' || data[i] == '1') {
            outFile << data[i];
        }
        else {
            int count = getGap(data, sizeof(data), data[i]); // number of digits between '+' or '-'
            decompress(string(&bits[i+1], &bits[i+count + 1]), data[i], outFile); //string of values between '+' or '-'
            i += count + 1; // Increment i by how much we skipped
        }
    }
}

int main(int argc, char *argv[]) {

    // Similar to #1, we must take in arguments: ./MyDecompress MyCompress_Compressed.txt Decompressed.txt    
    if(argc != 3) {
        cout << "INVALID ARGS: ./MyDecompress SOURCE DESTINATION\n"; 
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

    // Reading file
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
