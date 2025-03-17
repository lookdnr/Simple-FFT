#include "CUserInput.h"
#include <iostream>
#include <limits>

userInput::userInput(){ // constructor to avoid garbage values
    f_s = 0.;
    k = 1.;
}

void userInput::userParameters(){
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

double userInput::getfs(){ // getter for user params to be accessed in main
    return f_s;
}

double userInput::getk(){ // getter for user params to be accessed in main
    return k;
}