#ifndef ATOMTRACECL_GEOMETRY_H_
#define ATOMTRACECL_GEOMETRY_H_

#include "ObjectAttribute.h"

namespace AtomTraceCL
{
    enum GeometryType
    {
        SPHERE = 1,
        PLANE = 2,
        TRIANGLES = 3
    };
    class Geometry : public ObjectAttribute
    {
    };

} // namespace AtomTraceCL

#endif // ATOMTRACECL_GEOMETRY_H_