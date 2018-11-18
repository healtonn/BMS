/* 
 * File:   bms1A.cpp
 */

#include <cstdlib>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <string.h>

#include "sndfile.hh"

#define SAMPLE_RATE 32000
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQUENCY 1000
#define FREQ (1000.0 / SAMPLE_RATE)
#define SAMPLES_PER_BAUD (SAMPLE_RATE / FREQUENCY)    //one baud, one sinus

using namespace std;

const char* syncSequence = "00110011";
unsigned const int sequenceSize = strlen(syncSequence);

string getFileNameFromParameters(int argc, char** argv);
float setModifier(char* pair);

/* i am really sorry but i just run out of time
 * and i am not able to refactor my code into proper styling, please don't kill me O:)
 */
int main(int argc, char** argv) {
    string filename = getFileNameFromParameters(argc, argv);    //find file name

    ifstream inputFile;
    inputFile.open(filename);

    if(!inputFile){
        cerr << "Unable to open file " << filename << endl;
        exit(EXIT_FAILURE);
    }

    // read file content into buffer
    vector<char> bits;
    char c;
    while(inputFile >> c){
        bits.push_back(c);
    }
    inputFile.close();

    // insert synchronization sequence at the beginning of the message
    for(unsigned int i = 0; i <sequenceSize; i++)
        bits.insert(bits.begin(), syncSequence[sequenceSize - 1 - i]);
    
    /*for(unsigned int i = 0; i < bits.size(); i ++){
        cout << bits[i];
    }
    cout << endl;*/


    SndfileHandle outputFile;
    int bufferSize = SAMPLES_PER_BAUD * (bits.size() / 2); 
    int *buffer = new int[bufferSize];
    int signalValue;
    int bitPairIndex = 0;
    float modifier = 1.0;
    char tmp[3];    // store bit pairs here
    tmp[2] = '\0';
    for (int i = 0; i < bufferSize; i++){
        if(i % SAMPLES_PER_BAUD == 0){
            tmp[0] = bits[bitPairIndex];
            tmp[1] = bits[bitPairIndex + 1];
            bitPairIndex += 2;
            modifier = setModifier(tmp);
        }

        signalValue = modifier * AMPLITUDE * sin(FREQ * 2 * i * M_PI);

        buffer [i] = signalValue;
        //cout << "buffer: " << buffer[i] << endl;
    }

    string outputFileName = filename.substr(0, filename.find(".")) + ".wav";

    
    outputFile = SndfileHandle(outputFileName, SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);

    
    outputFile.write(buffer, bufferSize + 1);

    delete [] buffer;
    return EXIT_SUCCESS;
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

/*
 * Based upon bit pairs, set modifier  
 */
float setModifier(char* pair){
    string _pair(pair);
    //cout << "pair: " << _pair << endl;
    if(_pair == "00")      return 0.0;
    else if(_pair == "01") return 1.0/3.0;
    else if(_pair == "10") return 2.0/3.0;
    else                   return 1.0;
}

