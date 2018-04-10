#include "Transmission.h"

namespace AtomTraceCL
{
    Transmission::Transmission()
        : m_color(1.f, 1.f, 1.f), m_etaT(1.f)
    {}

    Transmission::Transmission(const float ietaT)
        : m_color(1.f, 1.f, 1.f), m_etaT(ietaT)
    {}

    Transmission::Transmission(const AtomMathCL::Vector3& iColor, const float ietaT)
        : m_color(iColor), m_etaT(ietaT)
    {}

    Transmission::~Transmission()
    {}

    __int32 Transmission::GetType() const
    {
        return TRANSMISSION;
    }

    unsigned __int32 Transmission::GetSize() const
    {
        return sizeof(Transmission) - sizeof(Material);
    }

    const void* Transmission::GetData() const
    {
        return &m_color;
    }

    void Transmission::SetEta(const float eta)
    {
        m_etaT = eta;
    }

    void Transmission::SetColor(const AtomMathCL::Vector3& color)
    {
        m_color = color;
    }

} // namespace AtomTraceCL
