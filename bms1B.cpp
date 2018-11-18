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
    int samplesPerSinus = sampleRate / FREQUENCY;       //samples per one "wave"
    cout << "ziskana samplerate: " << sampleRate << ", stanovena samlpes per sinus: " << samplesPerSinus << endl;
    cout << "amplituda: " << AMPLITUDE << endl;

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
        cout << "counter: " << sampleCounter << ", nacetl jsem: " << *sample << endl;
        if(*sample == 0 && firstZeros){
            numberOfFirstZeros++;
        }else if(*sample != 0 && firstZeros){
            cout << "prvnich nul nalezeno: " << numberOfFirstZeros << endl;
            firstZeros = false;
        }
        if(!zeroCheck && secondZeros && !firstZeros){
            numberOfNonZeros++;
        }

        if(*sample == 0 && lastValue == 0 && !firstZeros && !zeroCheck){
            cout << "nastavuji zeroCheck" << endl;
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
            cout << "result: " << result << endl;
           for(int i = 1; i < result; i++){
                inputFile.read(sample, 1);
                cout << "vyhodil jsem: " << *sample << endl;
            }
            break;
        }

    }
    samplesPerBaud = result;
    cout << "tmp1: " << numberOfFirstZeros << endl;
    cout << "tmp2: " << numberOfSecondZeros << endl;
    cout << "tmp3: " << numberOfNonZeros << endl;
    cout << "samples per baud: " << samplesPerBaud << endl;

    int maximum = 0;
    int i = 0;
    vector<int> bits;
    cout << "1/3 amplituda: " << THIRD_AMPLITUDE << endl;
    cout << "2/3 amplituda: " << TWO_THIRDS_AMPLITUDE << endl;
    cout << "amplituda: " << AMPLITUDE << endl;
    while(inputFile.read(sample,1)){
        i++;
        cout << "COUNTER: " << i << ", DEBUG: " << *sample << endl;
        if(maximum < *sample)
            maximum = abs(*sample);

        if(i % samplesPerBaud == 0){
            cout << "maximum: " << maximum << endl;
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
            cout << "resetuju pro vzorek: " << i << endl;
            maximum = 0;
        }
    }

    cout << "debug: ";
    for(int i = 0; i < bits.size(); i++){
        cout << bits[i];
    }
    cout << endl;
    string outputFileName = fileName.substr(0, fileName.find(".")) + ".txt";
    cout << "FILENAME: " << outputFileName << endl;

    ofstream output (outputFileName);
    if(output.is_open()){
        for(int i = 0; i < bits.size(); i++){
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

