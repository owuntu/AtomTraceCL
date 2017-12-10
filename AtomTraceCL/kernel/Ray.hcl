#ifndef ATOMTRACECL_RAY_HCL_
#define ATOMTRACECL_RAY_HCL_

#include "Transformation.hcl"

typedef struct
{
    float3 orig;
    float3 dir;
}Ray;

Ray RayTransformTo(const Transformation* pTf, const Ray* iray)
{
    Ray ray;
    ray.orig = TransformTo(pTf, &iray->orig);
    float3 ldest = iray->orig + iray->dir;
    ray.dir = (TransformTo(pTf, &(ldest)) - ray.orig);
    return ray;
}

#endif // ATOMTRACECL_RAY_HCL_
