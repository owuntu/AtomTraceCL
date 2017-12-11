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
        RenderObject() : m_pGm(nullptr), m_pMat(nullptr) {}
        ~RenderObject()
        {
            m_pGm = nullptr;
            m_pMat = nullptr;
        }

        void SetGeometry(Geometry* pGm);
        void SetMaterial(Material* pMat);

    public:
        Geometry* m_pGm;
        Material* m_pMat;

    }; // class RenderObject
} // namespace AtomTraceCL

#endif // ATOMTRACECL_RENDER_OBJECT_H_
