#include "CSignalProcessor.h"
#include <cmath>

// constructor: initialise pointers to avoid garbage values
signalProcessor::signalProcessor(){ 
    fileLen = 0;
    accSpectrum = nullptr;
    velSpectrum = nullptr;
    dispSpectrum = nullptr;
    velocityTime = nullptr;
    displacementTime = nullptr;

}

// setter for acceleration spectrum data
void signalProcessor::setAcc(double** input){
    accSpectrum = input;
    return;
}

// setter for updated file length
void signalProcessor::setFileLen(const int input){
    fileLen = input;
    return;
}

// funtion to perform integrtation in the Fourier domain
void signalProcessor::integrate(double samplingFrequency){

    // assign memory to vel and disp arrays
    velSpectrum = new double*[fileLen];
    dispSpectrum = new double*[fileLen];

    for (int i = 0; i < fileLen; i++){
        velSpectrum[i] = new double[2];
        dispSpectrum[i] = new double[2]; 
    }

    // now perform the integration using the property of the Fourier Tranform
    for (int i = 0; i < fileLen; i++){
        double fstep = i * samplingFrequency/fileLen; // frequency increment
        double factor = 2*M_PI*fstep; // 2pi f
        double sqrFactor = factor*factor; // -4pisqr fsqr

        if (fstep == 0) { // handle zero case to avoid div by 0
            velSpectrum[i][0] = 0;
            velSpectrum[i][1] = 0;
            dispSpectrum[i][0] = 0;
            dispSpectrum[i][1] = 0;
        }

        else { // if non-zero, use the property to modify in place
            velSpectrum[i][0] = accSpectrum[i][1] / factor; // real
            velSpectrum[i][1] = -accSpectrum[i][0] / factor; // imag

            dispSpectrum[i][0] = -accSpectrum[i][0] / sqrFactor;
            dispSpectrum[i][1] = -accSpectrum[i][1] / sqrFactor;
        }
    }
}

// function to allocate memory for the time domain outputs
void signalProcessor::timeDomainMemory(){
    velocityTime = new double*[fileLen];
    displacementTime = new double*[fileLen];

    for (int i = 0; i < fileLen; i++){
        velocityTime[i] = new double[2];
        displacementTime[i] = new double[2];
    }

    // copy data over. At this stage [:, 0] is real and [:, 1] is imag.
    for (int i = 0; i < fileLen; i++){
        velocityTime[i][0] = velSpectrum[i][0];
        displacementTime[i][0] = dispSpectrum[i][0];
        
        velocityTime[i][1] = velSpectrum[i][1];
        displacementTime[i][1] = dispSpectrum[i][1];
    } // now we have something to hold our inverse transforms.
}

// windowing function to reduce extreme values caused by small omega in the inverse transform (spectral leakage)
void signalProcessor::hannWindow(double windowParam){ 
    double window;
    for (int n = 0; n < fileLen; n++){
        window = windowParam*0.5*(1 - cos(2*M_PI*n/fileLen)); // k is user entered tappering constant. 1 if standard Hann window
        // multiply magnitude by window
        velSpectrum[n][0] *= window;
        dispSpectrum[n][0] *= window;
        velocityTime[n][0] *= window;
        displacementTime[n][0] *= window;
    }
}

// to receive Nx2 arrays with 1 row real 1 row imag and replace with 1 row magnitude 1 row frequency
void signalProcessor::computeAmplitudes(double** array, double f_s, int dirFlag){
    double realSqr, imagSqr, fStep, tStep;
    for (int i = 0; i < fileLen; i++){
        
        realSqr = pow(array[i][0], 2); // real portion
        imagSqr = pow(array[i][1], 2); // imag portion
        fStep = i * (f_s/fileLen); // frequency increment
        tStep = 1/f_s; // time increment

        if (dirFlag == 1) { // if transform in time domain
            array[i][1] = i*tStep; // only append time, leave real part unchanged
        }

        else{ // if transform in frequency domain
            array[i][0] = sqrt(realSqr + imagSqr); // compute mag for forward transf.
            array[i][1] = fStep; // compute freq bin and record for fwd transf.
        }
    }
}

// getter for velocity spectrum
double** signalProcessor::getVel(){
    return velSpectrum;
}

// getter for displacement spectrum
double** signalProcessor::getDisp(){
    return dispSpectrum;
}

// getter for velocity in time domain
double** signalProcessor::getVelTime(){
    return velocityTime;
}

// getter for displacement in time domain
double** signalProcessor::getDispTime(){
    return displacementTime;
}

// destructor to deallocate memory
signalProcessor::~signalProcessor(){ 
    
    for (int i = 0; i < fileLen; i++) {
        delete[] velSpectrum[i];
        delete[] dispSpectrum[i];
        delete[] velocityTime[i];
        delete[] displacementTime[i];
    }
    delete[] velSpectrum;
    delete[] dispSpectrum;
    delete[] velocityTime;
    delete[] displacementTime;
}