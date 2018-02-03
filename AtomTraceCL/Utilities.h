#ifndef __ATOMTRACE_UTILITIES_H_
#define __ATOMTRACE_UTILITIES_H_

#include <cl\cl_platform.h>
#include <istream>
#include <fstream>
#include <string>

bool CheckError(cl_int error, const std::string& msg = "");

void ReadFileToString(const std::string& name, std::string& out);

float GetRandomNumber01();

void ReadLine(std::ifstream& file, std::string& line);

#endif // __ATOMTRACE_UTILITIES_H_
