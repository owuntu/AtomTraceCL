#include <sstream>

#include "TriMesh.h"
#include "TriMeshHeader.h"
#include "Utilities.h"

namespace AtomTraceCL
{
    static inline void ReadVertex(const std::string& buffer, AtomMathCL::Vector3& vec)
    {
        std::stringstream sstream(buffer);
        sstream >> vec.X() >> vec.Y() >> vec.Z();
    }

    TriMesh::TriMesh() : m_pBuffer(nullptr)
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
        // TODO: add bvh size
        std::size_t size = m_bvh.m_numNodes * sizeof(cyBVH::Node)
                          + m_faces.size() * sizeof(unsigned __int32) // bvh elements list
                          + (sizeof(AtomMathCL::Vector3) * (m_vertices.size() + m_normals.size() + m_vtexture.size())
                          + sizeof(TriFace) * (m_faces.size() + m_ftexture.size() + m_fNormal.size()));
        size += sizeof(TriMeshHeader);
        return static_cast<unsigned __int32>(size);
    }

    const void* TriMesh::GetData() const
    {
        return m_pBuffer;
    }

    // Only handle faces with 3 or 4 vertices
    void TriMesh::ReadFaces(const std::string& buffer)
    {
        using namespace std;
        TriFace face;
        TriFace ft;
        TriFace fn;

        bool bft = false; // marker to check if ft or fn exist
        bool bfn = false;

        size_t faceVert = 0;
        std::stringstream sstream(buffer);
        while (!sstream.eof())
        {
            string str;
            sstream >> str;
            if (str.size() == 0)
            {
                break;
            }

            unsigned __int32 index = 0;
            int type = 0;

            for (std::size_t i = 0; i < str.size(); ++i)
            {
                if (str[i] <= '9' && str[i] >= '0')
                {
                    unsigned __int32 t = str[i] - '0';
                    index = index * 10 + t;
                }

                if ( (str[i] == '/' || i == str.size() - 1) )
                {
                    if (0 != index)
                    {
                        switch (type)
                        {
                            case 0: // face
                                face.v[faceVert] = index - 1;
                                break;
                            case 1:
                                ft.v[faceVert] = index - 1;
                                bft = true;
                                break;
                            case 2:
                                fn.v[faceVert] = index - 1;
                                bfn = true;
                                break;
                        }
                    }
                    index = 0;
                    type++;
                }
            }

            if (faceVert == 2)
            {
                m_faces.push_back(face);
                face.v[1] = face.v[2];
                if (bft)
                {
                    m_ftexture.push_back(ft);
                    ft.v[1] = ft.v[2];
                }

                if (bfn)
                {
                    m_fNormal.push_back(fn);
                    fn.v[1] = fn.v[2];
                }
            }
            else
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

        m_bvh.SetMesh(this);

        // Pack all data into m_buffer
        TriMeshHeader header;
        header.nodes.size = m_bvh.m_numNodes;
        header.elements.size = m_faces.size();
        header.vertices.size = m_vertices.size();
        header.vts.size = m_vtexture.size();
        header.vns.size = m_normals.size();
        header.faces.size = m_faces.size();
        header.fts.size = m_ftexture.size();
        header.fns.size = m_fNormal.size();

        header.nodes.index    = sizeof(header);
        header.elements.index = header.nodes.index    + header.nodes.size * sizeof(cyBVH::Node);
        header.vertices.index = header.elements.index + header.elements.size * sizeof(unsigned __int32);
        header.vts.index      = header.vertices.index + header.vertices.size * sizeof(AtomMathCL::Vector3);
        header.vns.index      = header.vts.index      + header.vts.size * sizeof(AtomMathCL::Vector3);
        header.faces.index    = header.vns.index      + header.vns.size * sizeof(AtomMathCL::Vector3);
        header.fts.index      = header.faces.index    + header.faces.size * sizeof(TriFace);
        header.fns.index      = header.fts.index      + header.fts.size * sizeof(TriFace);

        if (nullptr != m_pBuffer)
        {
            delete[] m_pBuffer;
        }
        m_pBuffer = new char[this->GetSize()];
        char* pCurr = m_pBuffer;

        // Copy TriMesh header
        CopyAndMovePtr<>(pCurr, header);

        // Pack BVH data
        // Copy BVH nodes and elements list
        CopyAndMovePtr<cyBVH::Node>(pCurr, m_bvh.nodes, m_bvh.m_numNodes);
        CopyAndMovePtr<unsigned __int32>(pCurr, m_bvh.elements, header.elements.size);

        // Copy vertices
        CopyAndMovePtr<Vector3>(pCurr, m_vertices);
        CopyAndMovePtr<Vector3>(pCurr, m_vtexture);
        CopyAndMovePtr<Vector3>(pCurr, m_normals);

        // Copy face index
        CopyAndMovePtr<TriFace>(pCurr, m_faces);
        CopyAndMovePtr<TriFace>(pCurr, m_ftexture);
        CopyAndMovePtr<TriFace>(pCurr, m_fNormal);

        return true;
    }

    void TriMesh::Clear()
    {
        m_vertices.clear();
        m_normals.clear();
        m_faces.clear();
        
        if (nullptr != m_pBuffer)
        {
            delete[] m_pBuffer;
            m_pBuffer = nullptr;
        }
    }

} // namespace AtomTraceCL
