#include "Geometry.h"
#include "RenderObject.h"

namespace AtomTraceCL
{
    void RenderObject::SetGeometry(Geometry* pGm)
    {
        m_pGm = pGm;
        m_gtype = pGm->GetType();
        m_gsize = pGm->GetSize();
    }

} // namespace AtomTraceCL
