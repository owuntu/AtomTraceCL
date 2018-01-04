#include "TriMesh.h"
#include "BVHTriMesh.h"

namespace AtomTraceCL
{
    void BVHTriMesh::SetMesh(const TriMesh *m, unsigned int maxElementsPerNode /* = CY_BVH_MAX_ELEMENT_COUNT */)
    {
        mesh = m;
        Clear();
        Build(mesh->m_faces.size(), maxElementsPerNode);
    }

    void BVHTriMesh::GetElementBounds(unsigned int i, float box[6]) const
    {
        using namespace AtomMathCL;
        const TriMesh::TriFace& f = mesh->m_faces[i];
        const Vector3& p = mesh->m_vertices[f.v[0]];
        box[0] = box[3] = p.X();
        box[1] = box[4] = p.Y();
        box[2] = box[5] = p.Z();
        for (int i = 1; i < 3; ++i) // for each vertex
        {
            const Vector3& pi = mesh->m_vertices[f.v[i]];
            for (int d = 0; d < 3; ++d) // for each dimension
            {
                if (box[d] > pi[d]) box[d] = pi[d]; // update min
                if (box[d + 3] < pi[d]) box[d + 3] = pi[d]; // update max
            }
        }
    }

    float BVHTriMesh::GetElementCenter(unsigned int i, int dim) const
    {
        const TriMesh::TriFace& f = mesh->m_faces[i];
        const std::vector<AtomMathCL::Vector3>& v = mesh->m_vertices;
        return (v[f.v[0]][dim] + v[f.v[1]][dim] + v[f.v[2]][dim]) / 3.0f;
    }

} // namespace AtomTraceCL
