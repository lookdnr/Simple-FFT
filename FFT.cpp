#include <iostream>
#include "CFileHandling.cpp"
#include "CUserInput.cpp"
#include "CFFT.cpp"
#include "CSignalProcessor.cpp"

using namespace std;

int main(){

    // create an instance of all neccesary class objects:
    FFT fft; 
    fileHandler fileHandling; 
    userInput input; 
    signalProcessor signal; 

    double* rawData; // raw data
    int fileLen, paddedLen; // raw & padded file length
    double f_s, k; // sampling frequency and windowing parameter
    // padded array, spectra, and time domain records:
    double** paddedData,** accSpectrum, ** velSpectrum, ** dispSpectrum, ** velTime, ** dispTime;

    fileHandling.getFile(); // get file to process and read in data
    rawData = fileHandling.getRawData(); // to pull raw data to pass to FFT
    fileLen = fileHandling.getFileLen(); // pull file length

    input.userParameters(); // request parameters from the user
    f_s = input.getfs(); // pull sampling frequency
    k = input.getk(); // pull windwing parameter

    fft.setFileLen(fileLen); // to set fileLen for FFT routine
    fft.zeroPadding(rawData, fileLen); // creates padded array and allocates memory for output array

    paddedData = fft.getAcc(); // pull padded array
    paddedLen = fft.getPaddedLen(); // pull updated array length

    signal.setFileLen(paddedLen); // set filelen in SP class
    fileHandling.setFileLen(paddedLen); // set fileLen in FH class

    fft.computeFFT(paddedData, paddedLen, -1); // compute the forward FFT
    accSpectrum = fft.getAcc(); // pull acc spectrum
    
    signal.setAcc(accSpectrum); // set the acceleration spectrum in SP class
    signal.integrate(f_s); // to compute velocity and displacement spectra
    signal.timeDomainMemory(); //allocate memory for vel/ disp time domain outputs
    
    // pull each of the remaining spectra and the time domain outputs
    velSpectrum = signal.getVel();
    dispSpectrum = signal.getDisp();
    velTime = signal.getVelTime();
    dispTime = signal.getDispTime();
    

    fft.computeFFT(velSpectrum, paddedLen, 1); // inv transform to obtain velocity-time
    fft.computeFFT(dispSpectrum, paddedLen, 1); // inv transform to obtain displacement-time
    
    // calculate magnitudes for the forward and inverse transforms
    signal.computeAmplitudes(accSpectrum, f_s, -1); // magnitudes for acceleration
    signal.computeAmplitudes(velSpectrum, f_s, -1); // magnitudes for velocity
    signal.computeAmplitudes(dispSpectrum, f_s, -1); // magnitudes for displacement

    signal.computeAmplitudes(velTime, f_s, 1); // magnitudes for vel-time
    signal.computeAmplitudes(dispTime, f_s, 1); // magnitudes for disp-time

    signal.hannWindow(k); // window to reduce edge effects 

    // write Fourier domain features
    fileHandling.writeFile(accSpectrum, "acc_spectrum"); // write magnitudes
    fileHandling.writeFile(velSpectrum, "vel_spectrum");
    fileHandling.writeFile(dispSpectrum, "disp_spectrum");

    // write time domain features
    fileHandling.writeFile(velTime, "velocity_time");
    fileHandling.writeFile(dispTime, "displacement_time");

    return 0;
}
