/*
C++ Coursework: FFT implementation
File: header file for the userInput class
Author: Luke Dinsdale
CID: 02285881

This is the header file which declares the member functions and variables for the userInput
class. The functions receive user inputted parameters which control the FFT routine
and post processing procedures.
*/

#pragma once

class userInput { // to deal with collecting user parameters
    public:
        userInput(); // constructor to initialise variables

        void userParameters(); // to request user parameters
        double getfs(); // getter for fs
        double getk(); // getter for k

    private:
        double f_s; // sampling frequency
        double k; // windowing parameter
};
