/* 
 * File:   bms1B.cpp
 */

#include <cstdlib>
#include <math.h>
#include <iostream>

#include "sndfile.hh"


using namespace std;

string getFileNameFromParameters(int argc, char** argv);


/*
 * 
 */
int main(int argc, char** argv) {
    
    SndfileHandle inputFile;
    int sampleRate;
    int *buffer;
    
    inputFile = SndfileHandle("sine.wav");
    
    sampleRate = inputFile.samplerate();
    cout << "ziskana samplerate: " << sampleRate << endl;
    
    buffer = new int[sampleRate];

    inputFile.read(buffer, sampleRate);
    //cout << inputFile.read(buffer, sampleRate) << endl;
    for (int i = 0; i < 52; i++){
        cout << "ziskane hodnoty: " << buffer[i] << endl;
    }
    
    
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

