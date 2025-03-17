/*
C++ Coursework: FFT implementation
File: implementation file for the userInput class
Author: Luke Dinsdale
CID: 02285881

This is the implementation file for the userInput class. The functions receive user 
inputted parameters which control the FFT routine and post processing procedures.
*/

#include "CUserInput.h"
#include <iostream>
#include <limits>

// constructor to avoid garbage values
userInput::userInput(){ 
    f_s = 0.;
    k = 1.;
}

// function to retrieve user params
void userInput::userParameters(){
    cout << "Enter the sampling frequency (Hz): ";
    
    while (!(cin >> f_s) || (f_s <= 0.)){ // if f_s not numeric or negative- keep looping till correct
        cout << endl << "s.f. must be a number >0 in Hz. Re-enter: ";
        cin.clear(); 
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // reset failbit from StackExchange (see documentation)
    }

    // general user info
    cout << endl << "This routine uses a Hann window to control spectral leakage." << endl;
    cout << "The windowing factor is k, where k >= 0. 1 is standard, 0 windows all data and 2 doubles the effect): ";
    cout << "Enter a windowing factor k (put 1 if unsure): ";
    
    while (!(cin >> k) || (k <= 0.)){ // if k not numeric or negative- keep looping till correct 
        cout << endl << "k must be a number greater than 0. Re-enter: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // reset failbit (after [2])
    }
}

// getter to share sampling freq
double userInput::getfs(){ // getter for user params to be accessed in main
    return f_s;
}

// getter to share tapering parameter
double userInput::getk(){ // getter for user params to be accessed in main
    return k;
}