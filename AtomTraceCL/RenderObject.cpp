#include "Material.h"
#include "Geometry.h"
#include "RenderObject.h"

namespace AtomTraceCL
{
    void RenderObject::SetGeometry(const Geometry* pGM)
    {
        m_pGM = pGM;
    }

    void RenderObject::SetMaterial(const Material* pMAT)
    {
        m_pMAT = pMAT;
    }


} // namespace AtomTraceCL
