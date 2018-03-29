#include <cstring>
#include <iostream>

#include "RenderObject.h"
#include "Geometry.h"
#include "Material.h"
#include "ObjectList.h"

namespace AtomTraceCL
{
    typedef struct
    {
        TransformPackBuffer transform;
                 __int32 gtype;
        unsigned __int32 gsize;
        unsigned __int32 geometryIndex; // Geometry index from the header
                 __int32 matType;
        unsigned __int32 matSize;
        unsigned __int32 matIndex;
    }ObjectHeader;

    ObjectList::ObjectList(unsigned long maxDataSize) :
        m_size(0), m_numObj(0), m_pData(nullptr), m_MAX_DATA_SIZE(maxDataSize)
    {
        m_pData = new char[m_MAX_DATA_SIZE];
        memset(m_pData, 0, m_MAX_DATA_SIZE);
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
        const Geometry& geo = *obj.m_pGM;
        const Material& mat = *obj.m_pMAT;
        header.gtype = geo.GetType();
        header.gsize = geo.GetSize();
        header.matType = mat.GetType();
        header.matSize = mat.GetSize();

        unsigned __int32 inc = sizeof(header)
                             + header.gsize // Geometry data
                             + header.matSize; // Material data
        if (m_size + inc >= m_MAX_DATA_SIZE)
        {
            std::cerr << "ERROR: ObjectList::AddObject: Not enough memory to add new object.\n";
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
