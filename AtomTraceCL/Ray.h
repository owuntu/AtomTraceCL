#ifndef __ATOMTRACE_CL_RAY_H_
#define __ATOMTRACE_CL_RAY_H_

#include "Vector3.h"

typedef struct Ray
{
    AtomMathCL::Vector3 m_orig;
    AtomMathCL::Vector3 m_dir;

    Ray();
}Ray;

#endif // __ATOMTRACE_CL_RAY_H_