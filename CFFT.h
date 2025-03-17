/*
C++ Coursework: FFT implementation
File: FFT class header file
Author: Luke Dinsdale
CID: 02285881

This is the header file which declares the member variables and functions
which implement the FFT algorithm.
*/

#pragma once
#include <filesystem>

using namespace std;
namespace fs = filesystem;

class FFT { // implements the main FFT routine 

    public:

        FFT(); // constructor to pull file
        ~FFT(); // destructor to save file and de-allocate

        // main FFT functions
        void zeroPadding(double* data, int arrayLen); // to pad accelerogram for FFT
        void paddedMemory(double* data, int paddedLen); // to allocate memory for padded array
        void computeFFT(double** input, int len, int dirFlag); // to execute FFT. -1 = fwd, 1 = inverse.

        int getPaddedLen(); // getter for updated array length
        double** getAcc(); // getter for acc spectrum
        void setFileLen(int input); // setter for file len

    private:
        int fileLen; // to store num lines
        // to store padded data (which becomes the acc spectra) and other spectra:
        double** padData,** velocitySpectrum,** displacementSpectrum; 

};
