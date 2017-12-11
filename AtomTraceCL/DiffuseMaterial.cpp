#include "DiffuseMaterial.h"

namespace AtomTraceCL
{
    DiffuseMaterial::DiffuseMaterial() :
        m_color(AtomMathCL::Vector3::ZERO)
    {}

    DiffuseMaterial::DiffuseMaterial(const AtomMathCL::Vector3& color) :
        m_color(color)
    {}

    __int32 DiffuseMaterial::GetType() const
    {
        return DIFFUSE;
    }

    unsigned __int32 DiffuseMaterial::GetSize() const
    {
        return sizeof(DiffuseMaterial) - sizeof(Material);
    }

    const void* DiffuseMaterial::GetData() const
    {
        return &m_color;
    }

    void DiffuseMaterial::SetColor(const AtomMathCL::Vector3& color)
    {
        m_color = color;
    }

} // namespace AtomTraceCL
