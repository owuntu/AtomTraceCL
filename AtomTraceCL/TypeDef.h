#ifndef ATOMTRACE_CL_TYPE_DEF_
#define ATOMTRACE_CL_TYPE_DEF_

#ifdef OPENCL_COMPILER
#define NAMESPACE_CPU_BEGIN
#define NAMESPACE_CPU_END
typedef uint CL_UINT;

#else

#define NAMESPACE_CPU_BEGIN namespace AtomTraceCL{
#define NAMESPACE_CPU_END }
typedef unsigned __int32 CL_UINT;

#endif // OPENCL_COMPILER

#endif // ATOMTRACE_CL_TYPE_DEF_
