#ifndef ATOMTRACE_CL_ARRAY_INFO_H_
#define ATOMTRACE_CL_ARRAY_INFO_H_

#include "TypeDef.h"

NAMESPACE_ATOMTRACECL_BEGIN

typedef struct
{
    CL_UINT index;
    CL_UINT size;  // number of elements in the array
}ArrayInfo;

NAMESPACE_END

#endif // ATOMTRACE_CL_ARRAY_INFO_H_
