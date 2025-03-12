#include <iostream>
#include "CFFT.cpp"

using namespace std;

int main(){

    FFT temp; // create an FFT object

    temp.getFile(); // get file to process

    temp.zeroPadding(); // creates padded array and allocates memory for output array
    temp.getUserParameters(); // request parameters for computations
    temp.computeFFT(temp.padData, temp.fileLen, -1); // compute the forward FFT

    temp.integrate(); // to compute velocity and displacement spectra

    temp.velDispMemory(); //allocate memory for vel/ disp time domain outputs

    temp.computeFFT(temp.velocitySpectra, temp.fileLen, 1); // inv transform to obtain velocity-time
    temp.computeFFT(temp.displacementSpectra, temp.fileLen, 1); // inv transform to obtain displacement-time

    // calculate magnitudes for the forward and inverse transforms
    double** accelerationSpectra = temp.computeAmplitudes(temp.padData, -1); // magnitudes for acceleration
    double** velocitySpectra = temp.computeAmplitudes(temp.velocitySpectra, -1); // magnitudes for velocity
    double** displacementSpectra = temp.computeAmplitudes(temp.displacementSpectra, -1); // magnitudes for displacement

    double** velocityTime = temp.computeAmplitudes(temp.velocityTime, 1); // magnitudes for vel-time
    double** displacementTime = temp.computeAmplitudes(temp.displacementTime, 1); // magnitudes for disp-time

    temp.hannWindow(); // window to reduce edge effects 

    // write Fourier domain features
    temp.writeFile(accelerationSpectra, "acc_spectrum"); // write magnitudes
    temp.writeFile(velocitySpectra, "vel_spectrum");
    temp.writeFile(displacementSpectra, "disp_spectrum");

    // write time domain features
    temp.writeFile(velocityTime, "velocity_time");
    temp.writeFile(displacementTime, "displacement_time");

    return 0;
}