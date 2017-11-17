#include <sstream>
#include <fstream>
#include <iostream>

#include <cl\cl.h>

#include "Utilities.h"

bool CheckError(cl_int error, const std::string& msg)
{
    bool ret = true;
    if (CL_SUCCESS != error)
    {
        std::cerr << msg << " failed,\t";
        std::cerr << "error code: " << error << "\n";
        ret = false;
    }
    return ret;
}

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
