#ifndef ATOMTRACECL_LIGHT_H_
#define ATOMTRACECL_LIGHT_H_

// AtomMathCL
#include "Vector3.h"

#include "Material.h"

namespace AtomTraceCL
{
    class Light : public Material
    {
    public:
        Light() : m_emission(AtomMathCL::Vector3::ZERO) {}
        Light(const AtomMathCL::Vector3& emission) : m_emission(emission) {}

        __int32 GetType() const
        {
            return LIGHT;
        }

        unsigned __int32 GetSize() const
        {
            return sizeof(Light) - sizeof(Material);
        }

        const void* GetData() const
        {
            return &m_emission;
        }

        void SetEmission(const AtomMathCL::Vector3& emission)
        {
            m_emission = emission;
        }

    private:
        AtomMathCL::Vector3 m_emission;
    };
} // namespace AtomTraceCL

#endif // ATOMTRACECL_LIGHT_H_
