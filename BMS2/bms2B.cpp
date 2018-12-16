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
    string fileName = getFileNameFromParameters(argc, argv);    //find file name
    string outputFileName = fileName + ".ok";

    ifstream inputFile;
    ofstream outputFile;
    inputFile.open(fileName.c_str(), std::ifstream::ate | std::ifstream::binary);
    outputFile.open(outputFileName.c_str());
    initialize_ecc();

    unsigned int inputFileSize = inputFile.tellg();      //input file in bytes
    inputFile.seekg(0, ios::beg);
    unsigned int inputFileChunks = inputFileSize / CHUNK_SIZE;
    //cout << "input file size: " << inputFileSize << " inut file chunks: " << inputFileChunks << endl;

    unsigned char* interlacedInputFileBuffer = new unsigned char[inputFileSize];
    unsigned char* inputFileBuffer = new unsigned char[inputFileSize];
    unsigned char* chunkBeingDecoded = new unsigned char[CHUNK_SIZE];
    unsigned char* decodedInputFile = new unsigned char[inputFileSize];
    inputFile.read((char*) interlacedInputFileBuffer, inputFileSize);

    for(int i = 0; i < CHUNK_SIZE; i++){
        for(unsigned int j = 0; j < inputFileChunks; j++){
            inputFileBuffer[(j * CHUNK_SIZE) + i] = interlacedInputFileBuffer[(i * inputFileChunks) + j];
        }
    }

    for(unsigned int i = 0; i < inputFileChunks; i++){
        for(int j = 0; j < CHUNK_SIZE; j++){
            chunkBeingDecoded[j] = inputFileBuffer[j + (i * CHUNK_SIZE)];
        }

        decode_data(chunkBeingDecoded, CHUNK_SIZE);
        if (check_syndrome () != 0) {
            correct_errors_erasures (chunkBeingDecoded, 
			     CHUNK_SIZE,
			     0, 
			     NULL);
        }

        for(int j = 0; j < CHUNK_SIZE; j++){
            decodedInputFile[j + (CHUNK_SIZE * i)] = chunkBeingDecoded[j];
        }
    }

    unsigned int lastModifiedChunk = decodedInputFile[0];
    unsigned int finalFileSize = ((inputFileChunks - 1) * ORIGINAL_BYTES) - lastModifiedChunk;
    //cout << "modifikovano bytu v poslednim chunku: " << lastModifiedChunk << " finalni velikost: " << finalFileSize - lastModifiedChunk << endl;

    unsigned char* finalFileContent = new unsigned char[finalFileSize];
    for(unsigned int i = 1; i < inputFileChunks; i++){
        if(i == (inputFileChunks - 1)){ // dont forget to shorten last chunk
            for(unsigned int j = 0; j < ORIGINAL_BYTES - lastModifiedChunk; j++){
                finalFileContent[(ORIGINAL_BYTES * (i - 1)) + j] = decodedInputFile[(i * CHUNK_SIZE) + j];
            }
        }else{
            for(int j = 0; j < ORIGINAL_BYTES; j++){
                finalFileContent[(ORIGINAL_BYTES * (i - 1)) + j] = decodedInputFile[(i * CHUNK_SIZE) + j];
            }
        }
    }

    outputFile.write((char*) finalFileContent, finalFileSize);
    inputFile.close();
    outputFile.close();
    delete [] interlacedInputFileBuffer;
    delete [] inputFileBuffer;
    delete [] chunkBeingDecoded;
    delete [] decodedInputFile;
    delete [] finalFileContent;

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