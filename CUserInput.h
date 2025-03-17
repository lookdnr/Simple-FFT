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
