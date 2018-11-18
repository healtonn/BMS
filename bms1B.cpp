/* 
 * File:   bms1B.cpp
 */

#include <cstdlib>
#include <math.h>
#include <iostream>
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
int main(int argc, char** argv) {
    
    SndfileHandle inputFile;
    string fileName = getFileNameFromParameters(argc, argv);
    int sampleRate;
    int *buffer;
    
    inputFile = SndfileHandle(fileName);
    sampleRate = inputFile.samplerate();
    int samplesPerSinus = sampleRate / FREQUENCY;       //samples per one "wave"
    cout << "ziskana samplerate: " << sampleRate << ", stanovena samlpes per sinus: " << samplesPerSinus << endl;
    cout << "amplituda: " << AMPLITUDE << endl;

    buffer = new int[sampleRate];

    //inputFile.read(buffer, sampleRate);
    int samplesPerBaud = 0;
    int sampleCounter = 0;
    int *sample = new int;
    *sample = 0;
    int test;
    /*while(true){          //baud begins with "0"
        sampleCounter++;
        test = inputFile.read(sample, 1);
        cout << "counter: " << sampleCounter << ", sample: " << *sample << endl;
        if(*sample != 0){
            if(samplesPerBaud == 0){
                samplesPerBaud = sampleCounter - 2;
                cout << "zjisteno vzorku na baud: " << samplesPerBaud << endl;
            }
        }

        if((samplesPerBaud != 0) && (sampleCounter % (4 * samplesPerBaud)) == 0){// times 4 because sync sequence is 4 bauds
            break;
        }
    }*/
    int tmp1 = 0;
    int result;
    bool tmp = true;
    bool tmmp = false;
    bool tmmp3 = true;
    int tmp2 = 0;
    int tmp3 = 0;
    int lastValue;
    while(true){
        lastValue = *sample;
        
        sampleCounter++;
        test = inputFile.read(sample, 1);
        cout << "counter: " << sampleCounter << ", nacetl jsem: " << *sample << endl;
        if(*sample == 0 && tmp){
            tmp1++;
        }else if(*sample != 0 && tmp){
            cout << "prvnich nul nalezeno: " << tmp1 << endl;
            tmp = false;
        }
        if(!tmmp && tmmp3 && !tmp){
            tmp3++;
        }

        if(*sample == 0 && lastValue == 0 && !tmp && !tmmp){
            cout << "nastavuji tmmp" << endl;
            tmmp = true;
            tmmp3 = false;
            tmp2++;
            tmp3 -= 2;
        }
        if(*sample == 0 && tmmp){
            tmp2++;
        }

        if(!tmp && tmmp && *sample != 0){
            result = (tmp1 + tmp2 + tmp3) / 3;
            cout << "result: " << result << endl;
           for(int i = 1; i < result; i++){
                inputFile.read(sample, 1);
                cout << "vyhodil jsem: " << *sample << endl;
            }
            break;
        }

    }
    samplesPerBaud = result;
    cout << "tmp1: " << tmp1 << endl;
    cout << "tmp2: " << tmp2 << endl;
    cout << "tmp3: " << tmp3 << endl;
    cout << "samples per baud: " << samplesPerBaud << endl;

    int maximum = 0;
    int i = 0;
    vector<int> bits;
    cout << "1/3 amplituda: " << THIRD_AMPLITUDE << endl;
    cout << "2/3 amplituda: " << TWO_THIRDS_AMPLITUDE << endl;
    cout << "amplituda: " << AMPLITUDE << endl;
    while(inputFile.read(sample,1)){
        i++;
        //cout << "toto nemuze fungovat: " << *sample << endl;
        if(maximum < *sample)
            maximum = abs(*sample);

        if(i % samplesPerBaud == 0){
            cout << "maximum: " << maximum << endl;
            if(maximum == 0){
                bits.push_back(0);
                bits.push_back(0);
            }else if(abs(maximum - THIRD_AMPLITUDE) < MAX_ERR){
                cout << "DEBUG: " << abs(maximum - THIRD_AMPLITUDE) << endl;
                bits.push_back(0);
                bits.push_back(1);
            }else if(abs(maximum - TWO_THIRDS_AMPLITUDE) < MAX_ERR){
                cout << "DEBUG: " << abs(maximum - TWO_THIRDS_AMPLITUDE) << endl;
                bits.push_back(1);
                bits.push_back(0);
            }else if(abs(maximum - AMPLITUDE) < MAX_ERR){
                cout << "DEBUG: " << abs(maximum - AMPLITUDE) << endl;
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

