#include "Plane.h"

using namespace AtomMathCL;
namespace AtomTraceCL
{
    Plane::Plane() :
        m_pos(Vector3::ZERO),
        m_normal(Vector3::UNIT_Y)
    {}

    Plane::Plane(const AtomMathCL::Vector3& ipos, const AtomMathCL::Vector3& inorm) :
        m_pos(ipos),
        m_normal(inorm)
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
        return &m_pos;
    }

} // namespace AtomTraceCL
