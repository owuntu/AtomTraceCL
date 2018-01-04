#ifndef ATOMTRACECL_BVH_TRIMESH_H_
#define ATOMTRACECL_BVH_TRIMESH_H_

#include "cyBVH.hpp"

namespace AtomTraceCL
{
    class TriMesh;

    class BVHTriMesh : public cyBVH
    {
    public:
        //!@name Constructors
        BVHTriMesh() : mesh(0) {}
        BVHTriMesh(const TriMesh *m) { SetMesh(m); }

        //! Sets the mesh pointer and builds the BVH structure.
        void SetMesh(const TriMesh *m, unsigned int maxElementsPerNode = CY_BVH_MAX_ELEMENT_COUNT);

    protected:
        //! Sets box as the i^th element's bounding box.
        virtual void GetElementBounds(unsigned int i, float box[6]) const;

        //! Returns the center of the i^th element in the given dimension.
        virtual float GetElementCenter(unsigned int i, int dim) const;

    private:
        const TriMesh *mesh;
    };
} // namespace AtomTraceCL

#endif // ATOMTRACECL_BVH_TRIMESH_H_
