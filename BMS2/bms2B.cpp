/*
 *  Decode input file
 */

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <cstdlib>

using namespace std;

string getFileNameFromParameters(int argc, char** argv);

int main(int argc, char** argv){
    string filename = getFileNameFromParameters(argc, argv);    //find file name

    ifstream inputFile;
    inputFile.open(filename.c_str());

    unsigned char c;
    while(inputFile >> c){
        cout << "debug: " << (int)c << endl;
    }

    inputFile.close();

    return 0;
}


string getFileNameFromParameters(int argc, char** argv){
    if(argc == 1){
        cerr << "Requires file name as parameter. Example: ./bms1A file.txt" << endl;
        exit(EXIT_FAILURE);
    } 
    else if(argc > 2) {
        cerr << "Too many parameters. Example: ./bms1A file.txt" << endl;
        exit(EXIT_FAILURE);
    }
    else
        return argv[1];
}