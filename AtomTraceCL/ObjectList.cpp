#include <cstring>

#include "RenderObject.h"
#include "Geometry.h"
#include "Material.h"
#include "ObjectList.h"

namespace AtomTraceCL
{
    typedef struct
    {
        TransformPackBuffer transform;
        unsigned __int32 gtype;
        unsigned __int32 gsize;
        unsigned __int32 geometryIndex; // Geometry index from the header
        unsigned __int32 matType;
        unsigned __int32 matSize;
        unsigned __int32 matIndex;
    }ObjectHeader;

    const static unsigned int MAX_DATA_SIZE = 65536; // size in byte

    ObjectList::ObjectList() :
        m_size(0), m_numObj(0), m_pData(nullptr)
    {
        m_pData = new char[MAX_DATA_SIZE];
        memset(m_pData, 0, MAX_DATA_SIZE);
        m_indexTable.clear();
    }

    ObjectList::~ObjectList()
    {
        if (nullptr != m_pData)
        {
            delete[] m_pData;
            m_pData = nullptr;
        }
        m_indexTable.clear();
    }

    bool ObjectList::AddObject(const RenderObject& obj)
    {
        char* pCurr = reinterpret_cast<char*>(m_pData) + m_size;
        ObjectHeader header;
        obj.PackTransformation(header.transform);
        const Geometry& geo = *obj.m_pGm;
        const Material& mat = *obj.m_pMat;
        header.gtype = geo.GetType();
        header.gsize = geo.GetSize();
        header.matType = mat.GetType();
        header.matSize = mat.GetSize();

        unsigned __int32 inc = sizeof(header)
                             + header.gsize // Geometry data
                             + header.matSize; // Material data
        if (m_size + inc >= MAX_DATA_SIZE)
        {
            return false;
        }
        m_indexTable.push_back(m_size);

        header.geometryIndex = m_size + sizeof(header);
        header.matIndex = header.geometryIndex + header.gsize;

        m_size += inc;
        m_numObj++;

        // Header
        memcpy(pCurr, &header, sizeof(header));
        pCurr += sizeof(header);

        // Geometry
        if (0 != header.gsize)
        {
            memcpy(pCurr, geo.GetData(), header.gsize);
            pCurr += header.gsize;
        }

        // Color, material
        if (0 != header.matSize)
        {
            memcpy(pCurr, mat.GetData(), header.matSize);
            pCurr += header.matSize;
        }


        return true;
    }
} // namespace AtomTraceCL
