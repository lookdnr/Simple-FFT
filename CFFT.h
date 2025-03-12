#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

class FFT {

    public:

        int fileLen; // to store num lines
        double** padData; // to store padded data (& eventually accelerationSpectra)
        double** velocitySpectra; // to store velocity spectrum
        double** displacementSpectra; // to store displacement spectra
        double** velocityTime; // to store velocity in time domain
        double** displacementTime; // to store displacement in time domain

        FFT(); // constructor to pull file
        FFT(const FFT &in); // copy constructor
        ~FFT(); // destructor to save file and de-allocate
        
        // main data movement functions
        void getFile(); // to request file
        void getFilenames(); // to list file names
        bool readFile(); // read file, return 
        void writeFile(double** accelerationSpectra, string outputName); // to write to file
        void getUserParameters();  // to obtain FFT, clipping, and filtering params

        // main FFT functions
        void allocateMemory(); // to allocate memory
        void zeroPadding(); // to pad accelerogram for FFT
        void paddedMemory(int nextPow); // to allocate memory for padded array
        void computeFFT(double** input, int len, int dirFlag); // to execute FFT. -1 = fwd, 1 = inverse.
        double** computeAmplitudes(double** array, int dirFlag); // calculate magnitude of each freq component
        void integrate(); // to perform integration in Fourier domain to get vel and disp spectra
        void hannWindow(); // to window the data to prevent spectral leakage

        // to compute velocity and displacement spectra
        void diff(); // to perform numerical differentiation 
        void velDispMemory(); // to allocate memory for vel and disp (time)
        void filter(); // to filter

    private:

        string filename; // for user input filename
        const string path = fs::current_path().string();
        double* initData; // for storing data pulled from file
        double f_s; // sampling frequency from user
        double k; // Hann window constant
        
};

