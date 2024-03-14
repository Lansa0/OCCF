#include <iostream>
#include <fstream>
#include "OCCF.h"

int main()
{
    OCCF Data;

    std::ifstream file("test.txt");
    if (!file.is_open())
    {
        std::cout << "File Failed to Open\n";
        return 1;
    }

    file >> Data; // Method to Parse a File

    std::string STRING = Data["Section"]["STRING"]; // = "Value"

    int INT = Data["Section"]["INT"];               //  = 2

    double DOUBLE = Data["Section"]["DOUBLE"];      // = 3.0

    bool BOOL = Data["Section"]["BOOL"];            // = true

}