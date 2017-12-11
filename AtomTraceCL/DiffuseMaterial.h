#ifndef ATOMTRACECL_DIFFUSE_MATERIAL_H_
#define ATOMTRACECL_DIFFUSE_MATERIAL_H_

// AtomMath
#include "Vector3.h"

#include "Material.h"

namespace AtomTraceCL
{
    class DiffuseMaterial : public Material
    {
    public:
        DiffuseMaterial();
        DiffuseMaterial(const AtomMathCL::Vector3& color);

        __int32 GetType() const;
        unsigned __int32 GetSize() const;
        const void* GetData() const;

        void SetColor(const AtomMathCL::Vector3& color);

    private:
        AtomMathCL::Vector3 m_color;
    };
} // namespace AtomTraceCL

#endif // ATOMTRACECL_DIFFUSE_MATERIAL_H_
