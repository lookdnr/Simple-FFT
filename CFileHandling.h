#pragma once

#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

class fileHandler {
    public:

        fileHandler(); // constructor to initialise variables
        ~fileHandler(); // destructor to deallocate

        void getFile(); // to request filename
        void getFilenames(); // to list filenames to user
        bool readFile(); // to read user specified file
        void writeFile(double** file, string outputName); // to write data to output file
        
        double* getRawData(); // getter for raw data
        int getFileLen(); // getter for fileLen
        void setFileLen(int input); // setter for fileLen

    private:
        string filename; // for user input filename
        const string path = (fs::path(__FILE__).parent_path() / "data").string(); // to set cwd
        double* rawData; // for storing raw data from file
        int fileLen; // to store number of data points
};