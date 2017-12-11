#include "Ray.h"
#include "Sphere.h"

namespace AtomTraceCL
{
    using namespace AtomMathCL;

    Sphere::Sphere()
    {
    }

    unsigned __int32 Sphere::GetSize() const
    {
        return sizeof(Sphere) - sizeof(Geometry);
    }

    unsigned __int32 Sphere::GetType() const
    {
        return SPHERE;
    }

    const void* Sphere::GetData() const
    {
        return (this);
    }

} // namespace AtomTraceCL
