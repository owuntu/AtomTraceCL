#ifndef ATOMTRACECL_MATERIAL_H_
#define ATOMTRACECL_MATERIAL_H_

#include "ObjectAttribute.h"

namespace AtomTraceCL
{
    enum MaterialType
    {
        LIGHT   = 0,
        DIFFUSE = 1
    };
    class Material : public ObjectAttribute
    {
    };
} // namespace AtomTraceCL

#endif // ATOMTRACECL_MATERIAL_H_
