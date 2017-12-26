#ifndef ATOMTRACE_CL_TRIMESH_H_
#define ATOMTRACE_CL_TRIMESH_H_

#include <string>
#include <vector>

#include "Vector3.h"

namespace AtomTraceCL
{

    class TriMesh
    {
    public:
        typedef struct
        {
            unsigned __int32 v[3]; // vertices index
        }TriFace;

    public:
        TriMesh();
        ~TriMesh();

        bool LoadObjFromFile(const std::string& fileName);

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

    };
} // namespace AtomTraceCL

#endif // ATOMTRACE_CL_TRIMESH_H_
