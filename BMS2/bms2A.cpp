/*
 *  Encode input file
 */

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <math.h>
#include "reedsolomon/ecc.h"

#define CHUNK_SIZE 255      //num of bytes of one msg chunk
#define ORIGINAL_BYTES 146

using namespace std;

string getFileNameFromParameters(int argc, char** argv);
unsigned int getChunkModification(int fileSize);

int main(int argc, char** argv){
    string fileName = getFileNameFromParameters(argc, argv);    //find file name

    ifstream inputFile;
    ofstream outputFile;

    string outputFileName = fileName + ".out";
    inputFile.open(fileName.c_str(), std::ifstream::ate | std::ifstream::binary);
    outputFile.open(outputFileName.c_str());
    
    unsigned int inputFileSize = inputFile.tellg();      //input file in bytes
    unsigned int numberOfInputChunks = (unsigned int)ceil((float)inputFileSize / ORIGINAL_BYTES);
    unsigned int numberOfFinalChunks = numberOfInputChunks + 1; //+ info chunk
    unsigned int lastInputChunkModif = getChunkModification(inputFileSize);
    unsigned int finalFileSize = (numberOfInputChunks + 1) * CHUNK_SIZE;   // output file size in bytes
    unsigned int inputFileEncodedContentSize = numberOfInputChunks * CHUNK_SIZE;
    //cout << "input file size: " <<  inputFileSize << " number of chunks input: " << numberOfInputChunks << " modif: " << lastInputChunkModif;
    //cout << " final: " << finalFileSize << " input file encoded: " << inputFileEncodedContentSize << endl;

    unsigned char* infoChunk = new unsigned char[CHUNK_SIZE];   //contains info about last last chunk
    unsigned char* inputFileEncodedContent = new unsigned char[inputFileEncodedContentSize];    //encoded stuff without info chunk
    unsigned char* outputFileEncodedContent = new unsigned char[finalFileSize]; // encoded stuff with encoded chunk
    unsigned char* inputBuffer = new unsigned char[inputFileSize + lastInputChunkModif];  //input file content buffer with modified encd chunk
    unsigned char* inputChunk = new unsigned char[ORIGINAL_BYTES];  // ORIGINAL BYTES of input chars
    unsigned char* outputChunk = new unsigned char[CHUNK_SIZE]; //CHUNK SIZE of encoded input bytes

    unsigned int i = 0;
    inputFile.seekg(0, ios::beg);
    inputFile.read((char*)inputBuffer, inputFileSize);
    for(unsigned int i = 0; i < lastInputChunkModif; i++){   //fill last chunk with zeros
        inputBuffer[inputFileSize + i] = 35;
    }

    for(unsigned int i = 0;i < CHUNK_SIZE; i++)
        infoChunk[i] = 42;

    initialize_ecc();

    infoChunk[0] = (unsigned char)lastInputChunkModif;  //encode info chunk and store it
    encode_data(infoChunk, ORIGINAL_BYTES, outputChunk);
    for(int i = 0; i < CHUNK_SIZE; i++)
        inputFileEncodedContent[i] = outputChunk[i];

    int inputchunkIndex = 0;
    int chunkIndex = 0;
    i = 0;
    for(; i < inputFileSize + lastInputChunkModif; i++){
        inputChunk[inputchunkIndex] = inputBuffer[i];   //load one char from input buffer into input chunk
        if(inputchunkIndex == ORIGINAL_BYTES - 1){  //one original byte chunk is ready, encode it
            encode_data(inputChunk, ORIGINAL_BYTES, outputChunk);
            for(int j = 0; j < CHUNK_SIZE; j++){
                inputFileEncodedContent[((chunkIndex + 1) * CHUNK_SIZE) + j] = outputChunk[j]; // +1 to offset info chunk
            }
            chunkIndex++;
            inputchunkIndex = -1;
        }

        inputchunkIndex++;
    }

    for(int i = 0; i < CHUNK_SIZE; i++){ //interlacing
        for(unsigned int j = 0; j < numberOfFinalChunks; j++){  // <= because of first info chunk
            outputFileEncodedContent[(numberOfFinalChunks * i) + j] = inputFileEncodedContent[i + (j * CHUNK_SIZE)];    //+1 for info chunk
        }

    }

    outputFile.write((char*)outputFileEncodedContent,finalFileSize);
    inputFile.close();
    outputFile.close();
    delete [] infoChunk; 
    delete [] inputFileEncodedContent;
    delete [] outputFileEncodedContent;
    delete [] inputBuffer;
    delete [] inputChunk;
    delete [] outputChunk;

    return 0;
}

/*
*   Return, by how much muste be last chunk of einput message increased
*/
unsigned int getChunkModification(int fileSize){
    unsigned int result = ORIGINAL_BYTES - (fileSize % ORIGINAL_BYTES);
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
