#ifndef __ATOM_TRACE_CL_SPHERE_H_
#define __ATOM_TRACE_CL_SPHERE_H_

#include "Vector3.h"

struct Ray;

class Sphere
{
public:
    Sphere();
    Sphere(float r, const AtomMathCL::Vector3& orig);

    bool Intersect(const Ray& ray);

private:
    AtomMathCL::Vector3 m_orig;
    float            m_radius;
};

#endif // __ATOM_TRACE_CL_SPHERE_H_