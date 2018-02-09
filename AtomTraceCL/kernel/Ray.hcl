#ifndef ATOMTRACECL_RAY_HCL_
#define ATOMTRACECL_RAY_HCL_

#include "Transformation.hcl"

#include "..\Ray.h"

Ray RayTransformTo(const Transformation* pTf, const Ray* iray)
{
    Ray ray;
    ray.m_orig = TransformTo(pTf, &iray->m_orig);
    float3 ldest = iray->m_orig + iray->m_dir;
    ray.m_dir = (TransformTo(pTf, &(ldest)) - ray.m_orig);
    return ray;
}

#endif // ATOMTRACECL_RAY_HCL_
