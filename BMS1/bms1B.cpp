/* 
 * File:   bms1B.cpp
 */

#include <cstdlib>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "sndfile.hh"

#define SAMPLE_RATE 32000       // disc. values per seconds - but i should aquire this from wav file
#define FREQUENCY 1000          // num. of "waves" per second 
#define AMPLITUDE (1.0 * 0x7F000000)  
#define THIRD_AMPLITUDE (1/3.0 * AMPLITUDE)
#define TWO_THIRDS_AMPLITUDE (2/3.0 * AMPLITUDE)
#define MAX_ERR 0x2FFFFFF           //max error for amplitude detection

using namespace std;

string getFileNameFromParameters(int argc, char** argv);



/*
 * 
 */
int main(int argc, char** argv) {   // i am really sorry but i just run out of time
    SndfileHandle inputFile;        // and i am not able to refactor my code into proper styling
    string fileName = getFileNameFromParameters(argc, argv);    // please don't kill me O:)
    int sampleRate;
    int *buffer;
    
    inputFile = SndfileHandle(fileName);
    sampleRate = inputFile.samplerate();

    buffer = new int[sampleRate];

    int samplesPerBaud = 0;
    int sampleCounter = 0;
    int *sample = new int;
    *sample = 0;
    int numberOfFirstZeros = 0;
    int result;
    bool firstZeros = true;
    bool zeroCheck = false;
    bool secondZeros = true;
    int numberOfSecondZeros = 0;
    int numberOfNonZeros = 0;
    int lastValue;
    while(true){
        lastValue = *sample;
        
        sampleCounter++;
        inputFile.read(sample, 1);
        if(*sample == 0 && firstZeros){
            numberOfFirstZeros++;
        }else if(*sample != 0 && firstZeros){
            firstZeros = false;
        }
        if(!zeroCheck && secondZeros && !firstZeros){
            numberOfNonZeros++;
        }

        if(*sample == 0 && lastValue == 0 && !firstZeros && !zeroCheck){
            zeroCheck = true;
            secondZeros = false;
            numberOfSecondZeros++;
            numberOfNonZeros -= 2;
        }
        if(*sample == 0 && zeroCheck){
            numberOfSecondZeros++;
        }

        if(!firstZeros && zeroCheck && *sample != 0){
            result = (numberOfFirstZeros + numberOfSecondZeros + numberOfNonZeros) / 3;
           for(int i = 1; i < result; i++){
                inputFile.read(sample, 1);
            }
            break;
        }

    }
    samplesPerBaud = result;

    int maximum = 0;
    int i = 0;
    vector<int> bits;
    while(inputFile.read(sample,1)){
        i++;
        if(maximum < *sample)
            maximum = abs(*sample);

        if(i % samplesPerBaud == 0){
            if(maximum == 0){
                bits.push_back(0);
                bits.push_back(0);
            }else if(abs(maximum - THIRD_AMPLITUDE) < MAX_ERR){
                bits.push_back(0);
                bits.push_back(1);
            }else if(abs(maximum - TWO_THIRDS_AMPLITUDE) < MAX_ERR){
                bits.push_back(1);
                bits.push_back(0);
            }else if(abs(maximum - AMPLITUDE) < MAX_ERR){
                bits.push_back(1);
                bits.push_back(1);
            }else{
                bits.push_back(2);
            }
            maximum = 0;
        }
    }

    /*for(int i = 0; i < bits.size(); i++){
        cout << bits[i];
    }
    cout << endl;*/
    string outputFileName = fileName.substr(0, fileName.find(".")) + ".txt";

    ofstream output (outputFileName);
    if(output.is_open()){
        for(unsigned int i = 0; i < bits.size(); i++){
            output << bits[i];
        }

        output.close();
    }else{
        cerr << "Connot open file " << outputFileName << endl;
    }
    
    
    delete [] buffer;
    return EXIT_SUCCESS;
}

string getFileNameFromParameters(int argc, char** argv){
    if(argc == 1){
        cerr << "Requires file name as parameter. Example: ./bms1B file.wav" << endl;
        exit(EXIT_FAILURE);
    } 
    else if(argc > 2) {
        cerr << "Too many parameters. Example: ./bms1B file.wav" << endl;
        exit(EXIT_FAILURE);
    }
    else
        return argv[1];
}

