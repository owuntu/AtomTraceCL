#include <iostream>
#include <fstream>

#include "TriMesh.h"

namespace AtomTraceCL
{
    // Utility functions
    static inline void ReadLine(std::ifstream& file, std::string& line)
    {
        char c;

    }


    TriMesh::TriMesh()
    {
        vertices.clear();
        normals.clear();
        faces.clear();
    }

    TriMesh::~TriMesh()
    {
        this->Clear();
    }

    bool TriMesh::LoadObjFromFile(const std::string& fileName)
    {
        using namespace std;
        ifstream objFile;
        objFile.open(fileName, ios::in);

        if (!objFile.is_open())
        {
            std::cerr << "Cannot open obj file: " << fileName << std::endl;
            return false;
        }



        objFile.close();
        return true;
    }

    void TriMesh::Clear()
    {
        vertices.clear();
        normals.clear();
        faces.clear();
    }

} // namespace AtomTraceCL
