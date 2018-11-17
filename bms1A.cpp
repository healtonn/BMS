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

#define SAMPLE_RATE 18000
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

/*
 * 
 */
int main(int argc, char** argv) {
    string filename = getFileNameFromParameters(argc, argv);    //find file name
    //cout << filename << endl;
    //cout << "test: " << sequenceSize << endl;

    ifstream inputFile;
    inputFile.open(filename);

    if(!inputFile){
        cout << "Unable to open file " << filename << endl;
        exit(EXIT_FAILURE);
    }

    // read file content into buffer
    vector<char> bits;
    char c;
    while(inputFile >> c){
        bits.push_back(c);
    }
    inputFile.close();

    // insert synchronization sequenc at the beginning of the message
    for (unsigned int i = 0; i <sequenceSize; i++)
        bits.insert(bits.begin(), syncSequence[sequenceSize - 1 - i]);
    
    for(unsigned int i = 0; i < bits.size(); i ++){
        cout << bits[i];
    }
    cout << endl;


    SndfileHandle outputFile;
    int bufferSize = SAMPLES_PER_BAUD * (bits.size() / 2); 
    cout << "SIZE: " << bufferSize << endl;
    cout << "pocet baudu: " << bits.size() / 2 << endl;
    cout << "samples per baud: " << SAMPLES_PER_BAUD << endl;
    int *buffer = new int[bufferSize];
    int signalValue;
    int bitPairIndex = 0;
    float modifier = 1.0;
    char tmp[3];    // store bit pairs here
    tmp[2] = '\0';
    for (int i = 0; i < bufferSize; i++){
        //cout << i  << " modulo: " << i % samplesPerBaud << endl;
        if(i % SAMPLES_PER_BAUD == 0){
            tmp[0] = bits[bitPairIndex];
            tmp[1] = bits[bitPairIndex + 1];
            bitPairIndex += 2;
            modifier = setModifier(tmp);
            //cout << "modifier set to: " << modifier << " pro dvojci " << tmp[0] << tmp[1] <<  endl;
            //usleep(500000);
        }

        signalValue = modifier * AMPLITUDE * sin(FREQ * 2 * i * M_PI);
        cout << "generovana hodnota: " << signalValue << endl;

        buffer [i] = signalValue;
        //cout << "buffer: " << buffer[i] << endl;
    }

    
    outputFile = SndfileHandle("sine.wav", SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);

    
    outputFile.write(buffer, bufferSize);

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

