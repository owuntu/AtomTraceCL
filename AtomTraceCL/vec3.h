#ifndef ATOMTRACE_CL_VEC3_H_
#define ATOMTRACE_CL_VEC3_H_

#ifdef OPENCL_COMPILER

typedef float3 vec3;

#else

#include "Vector3.h"
typedef AtomMathCL::Vector3 vec3;

#endif // OPENCL_COMPILER

#endif // ATOMTRACE_CL_VEC3_H_
