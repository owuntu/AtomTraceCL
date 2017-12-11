#ifndef ATOMTRACECL_PLANE_H_
#define ATOMTRACECL_PLANE_H_

#include "Vector3.h"
#include "Geometry.h"

namespace AtomTraceCL
{
    class Plane : public Geometry
    {
    public:
        Plane();

        unsigned __int32 GetSize() const;
        GeometryType GetType() const;
        const void* GetData() const;
    };
} // namespace AtomTraceCL

#endif // ATOMTRACECL_PLANE_H_