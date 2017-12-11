#include "Plane.h"

using namespace AtomMathCL;
namespace AtomTraceCL
{
    Plane::Plane()
    {}

    unsigned __int32 Plane::GetSize() const
    {
        return (sizeof(Plane) - sizeof(Geometry));
    }

    GeometryType Plane::GetType() const
    {
        return PLANE;
    }

    const void* Plane::GetData() const
    {
        return this;
    }

} // namespace AtomTraceCL
