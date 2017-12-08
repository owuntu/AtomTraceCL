#ifndef ATOMTRACECL_RENDER_OBJECT_H_
#define ATOMTRACECL_RENDER_OBJECT_H_

// AtomMathCL
#include "Vector3.h"

#include "Transformation.h"

namespace AtomTraceCL
{
    class Geometry;

    class RenderObject : public Transformation
    {
    public:
        RenderObject() : m_pGm(nullptr), m_emission(0.f), m_color(0.f) {}
        ~RenderObject()
        {
            m_pGm = nullptr;
        }

        void SetGeometry(Geometry* pGm);

    public:
        unsigned __int32 m_gtype; // geometry type
        unsigned __int32 m_gsize; // geometry size
        Geometry* m_pGm;

        // TODO: Use material interface.
        AtomMathCL::Vector3 m_emission;
        AtomMathCL::Vector3 m_color;

    }; // class RenderObject
} // namespace AtomTraceCL

#endif // ATOMTRACECL_RENDER_OBJECT_H_
