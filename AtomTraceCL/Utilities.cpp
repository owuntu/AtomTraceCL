#include <sstream>
#include <fstream>
#include <iostream>

#include "Utilities.h"

void ReadFileToString(const std::string& name, std::string& out)
{
    using namespace std;

    fstream file;
    file.open(name);

    if (!file.is_open())
    {
        file.close();
        std::cerr << "Cannot open file \"" << name << "\".\n";
        return;
    }

    stringstream sstr;
    sstr << file.rdbuf();
    out = sstr.str();

    file.close();
}