#ifndef __ATOM_TRACE_CL_SPHERE_H_
#define __ATOM_TRACE_CL_SPHERE_H_

#include "Geometry.h"

namespace AtomTraceCL
{

    class Sphere : public AtomTraceCL::Geometry
    {
    public:
        Sphere();

        unsigned __int32 GetSize() const;
        GeometryType GetType() const;
        const void* GetData() const;
    };

} // namespace AtomTraceCL

#endif // __ATOM_TRACE_CL_SPHERE_H_
