#ifndef __ATOMTRACE_CL_RAY_H_
#define __ATOMTRACE_CL_RAY_H_

#include "vec3.h"

typedef struct Ray
{
    vec3 m_orig;
    vec3 m_dir;
#ifndef OPENCL_COMPILER
    Ray();
#endif
}Ray;

#endif // __ATOMTRACE_CL_RAY_H_