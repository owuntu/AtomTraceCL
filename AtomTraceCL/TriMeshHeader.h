#ifndef ATOMTRACE_CL_TRIMESH_HEADER_H_
#define ATOMTRACE_CL_TRIMESH_HEADER_H_

#include "ArrayInfo.h"

NAMESPACE_CPU_BEGIN

typedef struct
{
    // bvh data
    ArrayInfo nodes;
    ArrayInfo elements;
    // triangle geometry data
    ArrayInfo vertices;
    ArrayInfo vts;
    ArrayInfo vns;
    ArrayInfo faces;
    ArrayInfo fts;
    ArrayInfo fns;
}TriMeshHeader;

NAMESPACE_CPU_END

#endif // ATOMTRACE_CL_TRIMESH_HEADER_H_
