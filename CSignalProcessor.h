/*
C++ Coursework: FFT implementation
File: header file for the signalProcessor class
Author: Luke Dinsdale
CID: 02285881

This is the header file whichd eclares the member variables and functions of 
the signalProcessor class. The functions perform numerical integration in the frequency
domain, window the data post-FFT, and compute the amplitudes of the data.
*/

#pragma once

class signalProcessor { // to manipulate the signal: integration, windowing, computing amplitudes
    public:

        signalProcessor(); // constructor to initialise vars
        ~signalProcessor(); // destructor to deallocate memory

        void integrate(double samplingFrequency); // to integrate in the Fourier domain
        void timeDomainMemory(); // to allocate memeory for time domain outputs
        void hannWindow(double windowParam); // to window data to prevent spectral leakage
        void computeAmplitudes(double** array, double f_s, int dirFlag); // calculate magnitude of each freq component

        double** getVel(); // getter for vel spectrum
        double** getDisp(); // getter for disp spectrum
        double** getVelTime(); // getter for vel in time domain
        double** getDispTime(); // getter for disp in time domain
        void setAcc(double** input); // setter for acceleration spectrum
        void setFileLen(const int input); // setter for filelen

    private:
        double** accSpectrum,** velSpectrum, ** dispSpectrum, ** velocityTime, **displacementTime;
        int fileLen;
};