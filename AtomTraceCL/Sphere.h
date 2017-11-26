#ifndef __ATOM_TRACE_CL_SPHERE_H_
#define __ATOM_TRACE_CL_SPHERE_H_

#include "Geometry.h"
#include "Vector3.h"

struct Ray;

class Sphere : public AtomTraceCL::Geometry
{
public:
    Sphere();
    Sphere(float r, const AtomMathCL::Vector3& orig);

    unsigned __int32 GetSize() const;

    bool Intersect(const Ray& ray);

private:
    float               m_radius;
    AtomMathCL::Vector3 m_orig;
    AtomMathCL::Vector3 m_emission;
    AtomMathCL::Vector3 m_color;
};

#endif // __ATOM_TRACE_CL_SPHERE_H_