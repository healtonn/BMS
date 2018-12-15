/*
 *  Encode input file
 */

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include "reedsolomon/ecc.h"

#define CHUNK_SIZE 255      //num of bytes of one msg chunk

using namespace std;

string getFileNameFromParameters(int argc, char** argv);
unsigned int getChunkModification(int fileSize);

int main(int argc, char** argv){
    string fileName = getFileNameFromParameters(argc, argv);    //find file name

    ifstream inputFile;
    ofstream outputFile;

    string outputFileName = fileName.substr(0, fileName.find(".")) + ".out";
    cout << "debug: " << outputFileName << endl;
    inputFile.open(fileName.c_str(), std::ifstream::ate | std::ifstream::binary);
    outputFile.open(outputFileName.c_str());
    
    unsigned int inputFileSize = inputFile.tellg();      //input file in bytes
    unsigned int lastChunkModif = getChunkModification(inputFileSize);
    unsigned int finalFileSize = inputFileSize + lastChunkModif;   // output file size in bytes
    cout << "input: " <<  inputFileSize << " modif: " << lastChunkModif << " final: " << finalFileSize << endl;

    unsigned char* infoChunk = new unsigned char[CHUNK_SIZE];

    *infoChunk = (char) lastChunkModif;
    int test = infoChunk[0];
    cout << "TEST: " << test << endl;
    outputFile.write((char*)infoChunk, sizeof(unsigned char) * CHUNK_SIZE);
    *infoChunk = (char) 42;
    outputFile.write((char*)infoChunk, sizeof(unsigned char) * CHUNK_SIZE);
    initialize_ecc();

    inputFile.close();
    outputFile.close();
    delete [] infoChunk;

    return 0;
}

/*
*   Return, by how much muste be last chunk of encoded message increased
*/
unsigned int getChunkModification(int fileSize){
    unsigned int result = CHUNK_SIZE - (fileSize % CHUNK_SIZE);
    return result;
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
