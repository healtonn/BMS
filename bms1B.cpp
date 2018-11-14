/* 
 * File:   bms1B.cpp
 */

#include <cstdlib>
#include <math.h>
#include <iostream>

#include "sndfile.hh"


using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    SndfileHandle inputFile;
    int sampleRate;
    float *buffer;
    
    inputFile = SndfileHandle("sine.wav");
    
    sampleRate = inputFile.samplerate();
    cout << "ziskana samplerate: " << sampleRate << endl;
    
    buffer = new float[sampleRate];

    //inputFile.read(buffer, sampleRate);
    cout << inputFile.read(buffer, sampleRate) << endl;
    for (int i = 0; i < sampleRate; i++){
        cout << "ziskane hodnoty: " << buffer[i] << endl;
    }
    
    
    delete [] buffer;
    return EXIT_SUCCESS;
}

