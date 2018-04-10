#ifndef ATOMTRACECL_TRANSMISSION_H_
#define ATOMTRACECL_TRANSMISSION_H_

#include "Vector3.h"

#include "Material.h"

namespace AtomTraceCL
{
    class Transmission : public Material
    {
    public:
        Transmission();
        Transmission(const float ietaT);
        Transmission(const AtomMathCL::Vector3& iColor, const float ietaT);
        
        ~Transmission();

        __int32 GetType() const;
        unsigned __int32 GetSize() const;
        const void* GetData() const;

        void SetEta(const float eta);
        void SetColor(const AtomMathCL::Vector3& color);

    private:
        AtomMathCL::Vector3 m_color; // default color
        float m_etaT; // index of refraction
    };
} // namespace AtomTraceCL

#endif // ATOMTRACECL_TRANSMISSION_H_
