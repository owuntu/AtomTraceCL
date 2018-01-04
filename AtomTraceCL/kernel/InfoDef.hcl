#ifndef ATOMTRACECL_INFODEF_HCL_
#define ATOMTRACECL_INFODEF_HCL_

#include "Transformation.hcl"

typedef struct
{
    Transformation transform;
    int  gtype;
    uint gsize;
    uint geometryIndex; // Geometry index from start address of the scene
    int  matType;  // Material information
    uint matSize;
    uint matIndex;
}ObjectHeader;

typedef struct
{
    uint index;
    uint size;
}ArrayInfo;

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

typedef struct
{
    float3 hitP;
    float3 hitN;
}HitInfoGeo; // Work around for an LLVM error. These two should be put into HitInfo

#endif // ATOMTRACECL_INFODEF_HCL_