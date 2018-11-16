/* 
 * File:   bms1A.cpp
 */

#include <cstdlib>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "sndfile.hh"

#define SAMPLE_RATE 100
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)

using namespace std;

const char* sync = "0011001100";

string getFileNameFromParameters(int argc, char** argv);
float setmodifier(char* pair);

/*
 * 
 */
int main(int argc, char** argv) {
    string filename = getFileNameFromParameters(argc, argv);    //find fie name
    cout << filename << endl;

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
    
    char tmp[2];    // store bit pairs here
    float modifier;
    for(unsigned int i = 0; i < bits.size(); i = i + 2){
        cout << bits[i] << bits[i + 1] << endl;
        tmp[0] = bits[i];
        tmp[1] = bits[i + 1];
        modifier = setmodifier(tmp);
    }
    cout << endl;

    inputFile.close();


    SndfileHandle outputFile;
    float *buffer = new float[SAMPLE_RATE];

    float test;
    for (int i = 0; i < SAMPLE_RATE; i++){
        test = AMPLITUDE * sin(FREQ * 2 * i * M_PI);
        cout << "generovana hodnota: " << test << endl;

        buffer [i] = test;
        cout << "buffer: " << buffer[i] << endl;
    }

    
    outputFile = SndfileHandle("sine.wav", SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);

    
    outputFile.write(buffer, SAMPLE_RATE);

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
 * Based upon bit pairs, move sin()  
 */
float setmodifier(char* pair){
    string _pair(2, *pair);
    //cout << "pair: " << _pair << endl;
    if(_pair == "00")      return 0.0;
    else if(_pair == "01") return 1/3;
    else if(_pair == "10") return 2/3;
    else                   return 1;
}

