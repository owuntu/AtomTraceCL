#ifndef ATOMTRACECL_RENDER_OBJECT_H_
#define ATOMTRACECL_RENDER_OBJECT_H_

// AtomMathCL
#include "Vector3.h"

#include "Transformation.h"

namespace AtomTraceCL
{
    class Geometry;
    class Material;

    class RenderObject : public Transformation
    {
    public:
        RenderObject() : m_pGM(nullptr), m_pMAT(nullptr) {}
        ~RenderObject()
        {
            m_pGM = nullptr;
            m_pMAT = nullptr;
        }

        void SetGeometry(const Geometry* pGM);
        void SetMaterial(const Material* pMAT);

    public:
        const Geometry* m_pGM;
        const Material* m_pMAT;

    }; // class RenderObject
} // namespace AtomTraceCL

#endif // ATOMTRACECL_RENDER_OBJECT_H_
