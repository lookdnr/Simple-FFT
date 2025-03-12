#include "CFFT.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cmath>

namespace fs = filesystem;

FFT::FFT(){ // constructor

    // initialise vars
    initData = nullptr;
    padData = nullptr;
    fileLen = 0;
    f_s = 0.;
    k = 0;

};

FFT::FFT(const FFT& in){ // copy constructor

    initData = in.initData;
    padData = in.padData;
    filename = in.filename;
}

void FFT::getFile(){
    // request and pull data from file
    cout << "Hello!" << endl << "Here are the files in /data:" << endl << endl;
    cout << endl << fs::current_path() << endl;
    getFilenames(); // list file names
    cout << endl << "Enter a file to proceed with: ";
    
    // do while to check file read correctly
    do{
    cin >> filename;
    } while (!readFile());
}

void FFT::getFilenames(){ // function to print filenames from dir
    
    for (const auto & entry : fs::directory_iterator(path)) // after [1]
        cout << entry.path().filename() << "\n";
}


bool FFT::readFile(){ // function to read files & allocate memory

    fstream dataFile;
    dataFile.open(path + filename, fstream::in);

    if (dataFile.fail()){
        cout << "Please re-enter filename: ";
        return false;
    }

    else{
        vector<double> tempData; // temp vector for quick swap
        double value;
        double _; // place holder for 1st column

        string skipLine;
        for (int i = 0; i < 5 && getline(dataFile, skipLine); i++); // to skip first 5 lines
    

        while (dataFile >> _ >> value) {  // read accelerations from file
            tempData.push_back(value); // append to vector
        }
        
        dataFile.close();
        fileLen = tempData.size();

        initData = new double[fileLen]; // allocate memory

        // give data to array
        for (size_t i = 0; i < fileLen; i++) {
            initData[i] = tempData[i];
        } 
        
        return true;
    }
    return false;
}

void FFT::getUserParameters(){
    cout << "Enter the sampling frequency (Hz): ";
    
    while (!(cin >> f_s) || (f_s <= 0.)){ // if f_s not numeric or negative- keep looping till correct
        cout << endl << "s.f. must be a number >0 in Hz. Re-enter: ";
        cin.clear(); 
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // reset failbit (after [2])
    }

    cout << endl << "This routine uses a Hann window to control spectral leakage." << endl;
    cout << "The tapering constant k controls the strength of the windowing, where k = 1 is the"; 
    cout << " standard window, 0 windows the entire data set, and 2 doubles the windowing effect." << endl;
    cout << "Enter a tapering factor k (put 1 if unsure): ";
    
    while (!(cin >> k) || (k <= 0.)){ // if k not numeric or negative- keep looping till correct 
        cout << endl << "k must be a number between 0 and 1. Re-enter: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // reset failbit (after [2])
    }
}

// Function to pad array with zeros
void FFT::zeroPadding(){ 

    double n = log2(fileLen); // log of base 2

    n = ceil(n); // round to next whole number
    const int nextPow = pow(2, n); // find next number to base 2
    paddedMemory(nextPow); // allocate memory and transfer data
    
    for (int i = fileLen; i < nextPow; i++){ // pad from end of data to next pow of 2
        padData[i][0] = 0; // pad with zeroes
        padData[i][1] = 0;
    }
    
}

void FFT::paddedMemory(int nextPow){
    padData = new double*[nextPow]; 
    
    for (int i = 0; i < nextPow; i++){
        padData[i] = new double[2];
    }

    for (int i = 0; i < fileLen; i++){
        padData[i][0] = initData[i];  // Transfer real data
        padData[i][1] = 0.;          // no imag data in input
    }

    delete[] initData; // de allocate memory for init data

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

    // Cooley-Tukey portion (conquer)
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

void FFT::velDispMemory(){
    velocityTime = new double*[fileLen];
    displacementTime = new double*[fileLen];

    for (int i = 0; i < fileLen; i++){
        velocityTime[i] = new double[2];
        displacementTime[i] = new double[2];
    }

    // copy data over. At this stage [:, 0] is real and [:, 1] is imag.
    for (int i = 0; i < fileLen; i++){
        velocityTime[i][0] = velocitySpectra[i][0];
        displacementTime[i][0] = displacementSpectra[i][0];
        
        velocityTime[i][1] = velocitySpectra[i][1];
        displacementTime[i][1] = displacementSpectra[i][1];
    } // now we have something to hold our inverse transf.
}

void FFT::integrate(){

    // assign memory to vel and disp arrays
    velocitySpectra = new double*[fileLen];
    displacementSpectra = new double*[fileLen];

    for (int i = 0; i < fileLen; i++){
        velocitySpectra[i] = new double[2];
        displacementSpectra[i] = new double[2]; 
    }

    // now perform the integration using the property of the Fourier Tranform
    for (int i = 0; i < fileLen; i++){
        double fstep = i * f_s/fileLen;
        double factor = 2*M_PI*fstep; // 2pi f
        double sqrFactor = factor*factor; // -4pisqr fsqr

        if (fstep == 0) { // handle zero case
            velocitySpectra[i][0] = 0;
            velocitySpectra[i][1] = 0;
            displacementSpectra[i][0] = 0;
            displacementSpectra[i][1] = 0;
        }

        else { 
            velocitySpectra[i][0] = padData[i][1] / factor; // real
            velocitySpectra[i][1] = -padData[i][0] / factor; // imag

            displacementSpectra[i][0] = -padData[i][0] / sqrFactor;
            displacementSpectra[i][1] = -padData[i][1] / sqrFactor;
        }
    }
}

// to receive Nx2 arrays with 1 row real 1 row imag and replace with 1 row magnitude 1 row frequency
double** FFT::computeAmplitudes(double** array, int dirFlag){
    double realSqr, imagSqr, fStep, tStep;
    for (int i = 0; i < fileLen; i++){
        
        realSqr = pow(array[i][0], 2);
        imagSqr = pow(array[i][1], 2);
        fStep = i * (f_s/fileLen);
        tStep = 1/f_s;

        if (dirFlag == 1) {
            array[i][1] = i*tStep; // only append time, leave real part unchanged
        }

        else{ 
            array[i][0] = sqrt(realSqr + imagSqr); // compute mag for forward transf.
            array[i][1] = fStep; // compute freq bin and record for fwd transf.
        }
    }
    return array;
}

// windowing function to reduce extreme values caused by small omega in the inverse transform
void FFT::hannWindow(){ 
    double window;
    for (int n = 0; n < fileLen; n++){
        window = k*0.5*(1 - cos(2*M_PI*n/fileLen)); // k is user entered tappering constant. 1 if standard Hann window
        velocitySpectra[n][0] *= window;
        displacementSpectra[n][0] *= window;
        velocityTime[n][0] *= window;
        displacementTime[n][0] *= window;
    }
}

void FFT::writeFile(double** input, string outputName){
    
    // check if results dir exists and make if not
    if (!fs::is_directory(path + filename + "_results")){
        fs::create_directory(path + filename + "_results");
        cout << "Directory " << filename << "_results created!";
    }

    fstream outFile;
    outFile.open(path + filename + "_results/" + outputName + ".txt", fstream::out);

    // write to files
    for (int i = 0; i < fileLen/2; i++) { // only write first half to avoid redundancies
        outFile << input[i][0] << '\t';
        outFile << input[i][1] << '\n';
    }
    outFile.close();

    return;
}

FFT::~FFT(){ // destructor 

    // de allocate memory now that we are done with it

    for (int i = 0; i < fileLen; i++) {
        delete[] padData[i];
        delete[] velocitySpectra[i];
        delete[] displacementSpectra[i];
        delete[] velocityTime[i];
        delete[] displacementTime[i];
    }
    delete[] padData;
    delete[] velocitySpectra;
    delete[] displacementSpectra; 
    delete[] velocityTime;
    delete[] displacementTime;
}
