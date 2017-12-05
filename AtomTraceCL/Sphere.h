#ifndef __ATOM_TRACE_CL_SPHERE_H_
#define __ATOM_TRACE_CL_SPHERE_H_

#include "Geometry.h"
#include "Vector3.h"

struct Ray;

namespace AtomTraceCL
{

    class Sphere : public AtomTraceCL::Geometry
    {
    public:
        Sphere();
        Sphere(float r, const AtomMathCL::Vector3& orig);

        unsigned __int32 GetSize() const;
        GeometryType GetType() const;
        const void* GetData() const;

        bool Intersect(const Ray& ray);

    private:
        float               m_radius;
        float padding1;
        float padding2;
        float padding3;
        AtomMathCL::Vector3 m_orig;
    };

} // namespace AtomTraceCL

#endif // __ATOM_TRACE_CL_SPHERE_H_
