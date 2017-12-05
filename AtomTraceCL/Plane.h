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
        Plane(const AtomMathCL::Vector3& ipos, const AtomMathCL::Vector3& inorm);

        unsigned __int32 GetSize() const;
        GeometryType GetType() const;
        const void* GetData() const;

    private:
        AtomMathCL::Vector3 m_pos;
        AtomMathCL::Vector3 m_normal;
    };
} // namespace AtomTraceCL

#endif // ATOMTRACECL_PLANE_H_