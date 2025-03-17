#pragma once
#include <iostream>

using namespace std;

class userInput{
    public:
        userInput(); // constructor to initialise variables

        void userParameters(); // to request user parameters
        double getfs(); // getter for fs
        double getk(); // getter for k

    private:
        double f_s; // sampling frequency
        double k; // windowing parameter
};
