#ifndef ATOMTRACE_CL_ARRAY_INFO_H_
#define ATOMTRACE_CL_ARRAY_INFO_H_

#include "TypeDef.h"

NAMESPACE_CPU_BEGIN

typedef struct
{
    CL_UINT index;
    CL_UINT size;
}ArrayInfo;

NAMESPACE_CPU_END

#endif // ATOMTRACE_CL_ARRAY_INFO_H_
