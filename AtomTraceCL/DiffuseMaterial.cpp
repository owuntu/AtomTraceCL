#include "DiffuseMaterial.h"

namespace AtomTraceCL
{
    DiffuseMaterial::DiffuseMaterial() :
        m_color(AtomMathCL::Vector3::ZERO),
        m_emission(AtomMathCL::Vector3::ZERO)
    {}

    DiffuseMaterial::DiffuseMaterial(const AtomMathCL::Vector3& color, const AtomMathCL::Vector3& emission):
        m_color(color),
        m_emission(emission)
    {}

    unsigned __int32 DiffuseMaterial::GetType() const
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

    void DiffuseMaterial::SetEmission(const AtomMathCL::Vector3& emission)
    {
        m_emission = emission;
    }

} // namespace AtomTraceCL
