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
    ray.dir = VectorTransformTo(pTf, &iray->dir);
    return ray;
}

#endif // ATOMTRACECL_RAY_HCL_
