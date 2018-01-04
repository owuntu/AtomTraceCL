#ifndef ATOMTRACE_CL_TRIMESH_H_
#define ATOMTRACE_CL_TRIMESH_H_

#include <string>
#include <vector>

#include "Geometry.h"
#include "Vector3.h"

#include "BVHTriMesh.h"

namespace AtomTraceCL
{

    class TriMesh : public Geometry
    {
    public:
        struct TriFace
        {
            unsigned __int32 v[3]; // vertices index
            unsigned __int32 padding;
            TriFace() { v[0] = v[1] = v[2] = 0; }
        };

    public:
        TriMesh();
        ~TriMesh();

        bool LoadObjFromFile(const std::string& fileName);

        __int32 GetType() const;
        unsigned __int32 GetSize() const;
        const void* GetData() const;

    private:
        void Clear();
        void ReadFaces(const std::string& buffer);

    public:
        std::vector<AtomMathCL::Vector3> m_vertices;
        std::vector<AtomMathCL::Vector3> m_normals;
        std::vector<AtomMathCL::Vector3> m_vtexture; // texture vertex
        std::vector<TriFace> m_faces;
        std::vector<TriFace> m_ftexture;
        std::vector<TriFace> m_fNormal;

        char* m_pBuffer;
        BVHTriMesh m_bvh;
    };

    template<typename T>
    inline void CopyAndMovePtr(char*& pDest, std::vector<T>& vec)
    {
        std::size_t tsize = vec.size() * sizeof(T);
        if (tsize != 0)
        {
            memcpy(pDest, &vec[0], tsize);
            pDest += tsize;
        }
    }

    template<typename T>
    inline void CopyAndMovePtr(char*& pDest, const T& val)
    {
        memcpy(pDest, &val, sizeof(val));
        pDest += sizeof(val);
    }

} // namespace AtomTraceCL

#endif // ATOMTRACE_CL_TRIMESH_H_
