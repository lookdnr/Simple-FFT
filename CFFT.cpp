#include "CFFT.h"
#include <iostream>
#include <cmath>

// constructor: initialise variables to avoid garbage values
FFT::FFT(){ 
    fileLen = 0;
    padData = nullptr;
    velocitySpectrum = nullptr;
    displacementSpectrum = nullptr;
};

// setter to retrieve fileLen from fileHandler routine
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

// function to allocate memory for padded array
void FFT::paddedMemory(double* data, int nextPow){ 
    padData = new double*[nextPow]; 
    
    for (int i = 0; i < nextPow; i++){
        padData[i] = new double[2];
    }

    for (int i = 0; i < fileLen; i++){
        padData[i][0] = data[i]; // Transfer real data
        padData[i][1] = 0.; // no imag data in input
    }

    fileLen = nextPow; // update array length record
}

void FFT::computeFFT(double** input, int len, int dirFlag) { // implementation after [3]

    // first, divide: bit reversal routine
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

    // next, conquer: compute twiddle factors and apply in butterfly operations 
    for (int step = 2; step <= len; step *= 2) { // step: size of merging subarray
        
        double angle = dirFlag * (2 * M_PI / step); // phase angle. note dirFlag is -1 for fwd, 1 for inverse
        double w_real = cos(angle); // real part of twiddle
        double w_imag = sin(angle); // imag part of twiddle
        // u_: current twiddle factor, 
        // temp_real/imag: temp storage for multiplied components
        // u_w_: temp storage for updated twiddle 
        double u_real, u_imag, temp_real, temp_imag, temp_w_real;
        int index1, index2; // to index the array

        for (int k = 0; k < len; k += step) {
            u_real = 1.0;
            u_imag = 0.0;
            for (int j = 0; j < step / 2; j++) {
                index1 = k + j; // even element
                index2 = k + j + step / 2; // odd element

                // twiddle * odd part: temp_ hold odd element adjusted by twiddle multiplication
                temp_real = u_real * input[index2][0] - u_imag * input[index2][1];
                temp_imag = u_real * input[index2][1] + u_imag * input[index2][0];

                // butterfly operation: in-place updates
                input[index2][0] = input[index1][0] - temp_real;
                input[index2][1] = input[index1][1] - temp_imag;
                input[index1][0] += temp_real;
                input[index1][1] += temp_imag;

                // update twiddle factor
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

// getter to share updated len with other classes
int FFT::getPaddedLen(){
    return fileLen;
}

// getter to share acc spectrum with other classes
double** FFT::getAcc(){
    return padData;
}

// destructor to deallocate memory
FFT::~FFT(){ 

    for (int i = 0; i < fileLen; i++) {
        delete[] padData[i];
    }
    delete[] padData;
}