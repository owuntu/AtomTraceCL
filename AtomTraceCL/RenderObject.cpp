#include "Material.h"
#include "Geometry.h"
#include "RenderObject.h"

namespace AtomTraceCL
{
    void RenderObject::SetGeometry(Geometry* pGm)
    {
        m_pGm = pGm;
    }

    void RenderObject::SetMaterial(Material* pMat)
    {
        m_pMat = pMat;
    }


} // namespace AtomTraceCL
