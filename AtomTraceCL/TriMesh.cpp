#include <iostream>
#include <fstream>
#include <sstream>

#include "TriMesh.h"

namespace AtomTraceCL
{
    // Utility functions
    static inline void ReadLine(std::ifstream& file, std::string& line)
    {
        char c;
        line.clear();
        file.read(&c, sizeof(c));
        while (c != '\n' && !file.eof())
        {
            line += c;
            file.read(&c, sizeof(c));
        }
    }

    static inline void ReadVertex(const std::string& buffer, AtomMathCL::Vector3& vec)
    {
        std::stringstream sstream(buffer);
        sstream >> vec.X() >> vec.Y() >> vec.Z();
    }

    TriMesh::TriMesh() : m_buffer(nullptr)
    {
        m_vertices.clear();
        m_normals.clear();
        m_faces.clear();
    }

    TriMesh::~TriMesh()
    {
        this->Clear();
    }

    __int32 TriMesh::GetType() const
    {
        return TRIANGLES;
    }

    unsigned __int32 TriMesh::GetSize() const
    {
        std::size_t size = (sizeof(AtomMathCL::Vector3) * (m_vertices.size() + m_normals.size() + m_vtexture.size())
                          + sizeof(TriFace) * (m_faces.size() + m_ftexture.size() + m_fNormal.size()));
        size += sizeof(unsigned __int32) * 2; // storage for number of vertices and number of faces.
        size += sizeof(unsigned __int32) * 2; // storage for whether it contain normals and texture coordinates data.
        return static_cast<unsigned __int32>(size);
    }

    const void* TriMesh::GetData() const
    {
        return m_buffer;
    }

    void TriMesh::ReadFaces(const std::string& buffer)
    {
        using namespace std;
        TriFace face;
        TriFace ft;
        TriFace fn;

        size_t faceVert = 0;
        std::stringstream sstream(buffer);
        while (!sstream.eof())
        {
            if (faceVert == 3)
            {
                m_faces.push_back(face);
                m_ftexture.push_back(ft);
                m_fNormal.push_back(fn);

                face.v[1] = face.v[2];
                ft.v[1] = ft.v[2];
                fn.v[1] = fn.v[2];
                faceVert = 2;
            }

            string str;
            sstream >> str;

            unsigned __int32 index = 0;
            int type = 0;

            for (std::size_t i = 0; i < str.size(); ++i)
            {
                if (str[i] == '/' || i == str.size() - 1)
                {
                    switch (type)
                    {
                        case 0: // face
                            face.v[faceVert] = index-1;
                            break;
                        case 1:
                            ft.v[faceVert] = index-1;
                            break;
                        case 2:
                            fn.v[faceVert] = index-1;
                            break;
                    }

                    index = 0;
                    type++;
                    continue;
                }

                if (str[i] <= '9' && str[i] >= '0')
                {
                    unsigned __int32 t = str[i] - '0';
                    index = index * 10 + t;
                }
            }
            faceVert++;
        }
    }

    bool TriMesh::LoadObjFromFile(const std::string& fileName)
    {
        using namespace std;
        using namespace AtomMathCL;
        ifstream objFile;
        objFile.open(fileName, ios::in);

        if (!objFile.is_open())
        {
            std::cerr << "Cannot open obj file: " << fileName << std::endl;
            return false;
        }

        this->Clear();

        std::string line;
        while (!objFile.eof())
        {
            ReadLine(objFile, line);
            // skip leading space
            std::size_t i = 0;
            for (; i < line.size(); ++i)
            {
                if (line[i] != ' ')
                    break;
            }
            
            // skip comments
            if (i == line.size() || line[i] == '#')
                continue;

            char c = line[i];
            switch (c)
            {
                case 'v':
                {
                    ++i;
                    char sc = line[i];
                    if (sc == 'n' || sc == 't') // normal or texture vertices
                    {
                        ++i;
                    }
                    Vector3 vert;
                    ReadVertex(line.substr(i, line.size() - i), vert);

                    switch (sc)
                    {
                    case 'n':
                        m_normals.push_back(vert);
                        break;
                    case 't':
                        m_vtexture.push_back(vert);
                        break;
                    default:
                        m_vertices.push_back(vert);
                        break;
                    }
                    break;
                }

                case 'f':
                {
                    ++i;
                    this->ReadFaces(line.substr(i, line.size()-i));
                    break;
                }
                default:
                    break;
            }
        }

        objFile.close();

        // pack all data into m_buffer
        std::size_t numVert = m_vertices.size();
        std::size_t numFace = m_faces.size();

        return true;
    }

    void TriMesh::Clear()
    {
        m_vertices.clear();
        m_normals.clear();
        m_faces.clear();
        
        if (nullptr != m_buffer)
        {
            delete[] m_buffer;
            m_buffer = nullptr;
        }
    }

} // namespace AtomTraceCL
