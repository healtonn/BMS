/*
 *  Decode input file
 */

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include "reedsolomon/ecc.h"

#define CHUNK_SIZE 255      //num of bytes of one msg chunk
#define ORIGINAL_BYTES 146

using namespace std;

string getFileNameFromParameters(int argc, char** argv);

int main(int argc, char** argv){
    string filename = getFileNameFromParameters(argc, argv);    //find file name

    ifstream inputFile;
    inputFile.open(filename.c_str());

    initialize_ecc();

    unsigned char* test = new unsigned char[CHUNK_SIZE];
    inputFile.read((char*) test, CHUNK_SIZE);
    cout << "NACTENO: ";
    for(int i=0; i< CHUNK_SIZE; i++){
        cout << (int) test[i]<< ", " ;
    }
    cout << endl;
    decode_data(test, CHUNK_SIZE);

    /* check if syndrome is all zeros */
    if (check_syndrome () != 0) {
        correct_errors_erasures (test, 
			     CHUNK_SIZE,
			     0, 
			     NULL);
 
        //printf("Corrected codeword: \"%s\"\n", test);
    }

    cout << "DEKODOVANO: ";
    for(int i=0; i< CHUNK_SIZE; i++){
        cout << (int) test[i] << " i: " << i << ", " ;
    }
    cout << endl;

    /*unsigned char c;
    while(inputFile >> c){
        cout << "debug: " << (int)c << endl;
    }*/

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