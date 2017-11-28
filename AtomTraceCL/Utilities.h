#ifndef __ATOMTRACE_UTILITIES_H_
#define __ATOMTRACE_UTILITIES_H_

#include <cl\cl_platform.h>
#include <string>

bool CheckError(cl_int error, const std::string& msg = "");

void ReadFileToString(const std::string& name, std::string& out);

float GetRandomNumber01();

#endif // __ATOMTRACE_UTILITIES_H_
