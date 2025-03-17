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