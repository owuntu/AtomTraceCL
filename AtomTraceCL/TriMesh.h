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

    public:
        std::vector<AtomMathCL::Vector3> vertices;
        std::vector<AtomMathCL::Vector3> normals;
        std::vector<TriFace> faces;

    };
} // namespace AtomTraceCL

#endif // ATOMTRACE_CL_TRIMESH_H_
