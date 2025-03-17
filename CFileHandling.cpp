#include "CFileHandling.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <vector>

using namespace std;

namespace fs = filesystem;

fileHandler::fileHandler(){ // initialise vars to avoid garbage values
    rawData = nullptr;
    fileLen = 0;
}

void fileHandler::getFile(){
    // request and pull data from file
    cout << "Hello!" << endl << "Here are the files in /data:" << endl << endl;
    getFilenames(); // list file names
    cout << endl << "Enter a file to proceed with: ";
    

    // do while to check file read correctly
    do{
    cin >> filename;
    } while (!readFile());
}

void fileHandler::getFilenames(){ // function to print filenames from dir
    
    for (const auto & entry : fs::directory_iterator(path)) // after [1]
        cout << entry.path().filename() << "\n";
}

bool fileHandler::readFile(){ // function to read files & allocate memory

    fstream dataFile;
    dataFile.open(path + "/" + filename, fstream::in);

    if (dataFile.fail()){
        cout << "Please re-enter filename: ";
        return false;
    }

    else{
        vector<double> tempData; // temp vector for quick swap and file length determination
        double value;
        double _; // place holder for 1st column

        string skipLine;
        for (int i = 0; i < 5 && getline(dataFile, skipLine); i++); // to skip first 5 lines of metadata
    
        while (dataFile >> _ >> value) {  // read accelerations from file
            tempData.push_back(value); // append to vector
        }
        
        dataFile.close();
        fileLen = tempData.size(); // determine number of data points

        rawData = new double[fileLen]; // allocate memory for raw data array

        // give data to array
        for (size_t i = 0; i < fileLen; i++) {
            rawData[i] = tempData[i];
        } 
        
        return true;
    }
    return false;
}

void fileHandler::setFileLen(int input){
    fileLen = input;
    return;
}

void fileHandler::writeFile(double** input, string outputName){
    
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

double* fileHandler::getRawData(){
    return rawData;
}

int fileHandler::getFileLen(){
    return fileLen;
}

fileHandler::~fileHandler(){
    delete[] rawData; // de allocate memory for init data
}