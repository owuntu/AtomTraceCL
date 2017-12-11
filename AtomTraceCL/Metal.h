#ifndef ATOMTRACECL_METAL_H_
#define ATOMTRACECL_METAL_H_

// AtomMathCL
#include "Vector3.h"

#include "Material.h"

namespace AtomTraceCL
{
    class Metal : public Material
    {
    public:
        Metal() : m_eta(AtomMathCL::Vector3::ZERO), m_kk(AtomMathCL::Vector3::ZERO)
        {}

        Metal(const AtomMathCL::Vector3& eta, const AtomMathCL::Vector3& kk) :
            m_eta(eta), m_kk(kk)
        {}

        inline __int32 GetType() const
        {
            return METAL;
        }

        inline unsigned __int32 GetSize() const
        {
            return sizeof(Metal) - sizeof(Material);
        }

        inline const void* GetData() const
        {
            return &m_eta;
        }

        AtomMathCL::Vector3 m_eta;
        AtomMathCL::Vector3 m_kk;
    };
} // namespace AtomTraceCL

#endif // ATOMTRACECL_METAL_H_
