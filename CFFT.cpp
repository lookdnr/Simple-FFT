#include "CFFT.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cmath>

namespace fs = filesystem;

FFT::FFT(){ // constructor 
    fileLen = 0;
};

FFT::FFT(const FFT& in){ // copy constructor

}

void FFT::setFileLen(int input){
    fileLen = input;
    return;
}

// Function to pad array with zeros
void FFT::zeroPadding(double* data, int arrayLen) { 
    double n = log2(arrayLen); // log base 2 of fileLen
    int int_n = (int)round(n); // nearest integer to n
    int nextPow = arrayLen; // default value for no padding

    if (arrayLen != (int)pow(2, int_n)) { // if fileLen is not a power of 2
        int_n = ceil(n); // round up to next integer
        nextPow = pow(2, int_n); // find next power of 2
        paddedMemory(data, nextPow); // allocate memory for padded size
    } 
    
    else { // if fileLen is already a power of 2
        paddedMemory(data, nextPow); // allocate memory for current size
    }
    
    // loop and pad with zeros from the original file len to the end
    for (int i = fileLen; i < nextPow; i++) {
        padData[i][0] = 0; // pad real part with zero
        padData[i][1] = 0; // pad imaginary part with zero
    }
}

void FFT::paddedMemory(double* data, int nextPow){
    padData = new double*[nextPow]; 
    
    for (int i = 0; i < nextPow; i++){
        padData[i] = new double[2];
    }

    for (int i = 0; i < fileLen; i++){
        padData[i][0] = data[i];  // Transfer real data
        padData[i][1] = 0.;          // no imag data in input
    }

    fileLen = nextPow; // update array length record
}

void FFT::computeFFT(double** input, int len, int dirFlag) { // implementation after [3]

    // bit reversal routine (divide)
    int j = 0;
    for (int i = 1; i < len; i++) {
        int bit = len >> 1;
        while (j >= bit) {
            j -= bit;
            bit >>= 1;
        }
        j += bit;
        if (i < j) {
            swap(input[i][0], input[j][0]); // swap real 
            swap(input[i][1], input[j][1]); // swap imag
        }
    }

    // (conquer)
    for (int step = 2; step <= len; step *= 2) {
        double angle = dirFlag * (2 * M_PI / step); // note dirFlag is -1 for fwd, 1 for inverse
        double w_real = cos(angle);
        double w_imag = sin(angle);
        double u_real, u_imag, temp_real, temp_imag, temp_w_real;
        int index1, index2;

        for (int k = 0; k < len; k += step) {
            u_real = 1.0;
            u_imag = 0.0;
            for (int j = 0; j < step / 2; j++) {
                index1 = k + j;
                index2 = k + j + step / 2;

                // twiddle * odd part
                temp_real = u_real * input[index2][0] - u_imag * input[index2][1];
                temp_imag = u_real * input[index2][1] + u_imag * input[index2][0];

                // butterfly op
                input[index2][0] = input[index1][0] - temp_real;
                input[index2][1] = input[index1][1] - temp_imag;
                input[index1][0] += temp_real;
                input[index1][1] += temp_imag;

                // update twiddle
                temp_w_real = u_real * w_real - u_imag * w_imag;
                u_imag = u_real * w_imag + u_imag * w_real;
                u_real = temp_w_real;
            }
        }
    }
    if (dirFlag == 1){ // 1/N normalisation for IFFT
        for (int i = 1; i < fileLen; i++){
            input[i][0] /= fileLen;
            input[i][1] /= fileLen;
        }
    }
}

int FFT::getPaddedLen(){
    return fileLen;
}

double** FFT::getAcc(){
    return padData;
}

FFT::~FFT(){ // destructor to deallocate memory

    for (int i = 0; i < fileLen; i++) {
        delete[] padData[i];
    }
    delete[] padData;
}
