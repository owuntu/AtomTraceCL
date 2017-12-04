#include <cstring>

#include "Sphere.h"
#include "ObjectList.h"

namespace AtomTraceCL
{
    const static unsigned int MAX_DATA_SIZE = 65536; // size in byte

    ObjectList::ObjectList() :
        m_size(0), m_numObj(0), m_pData(nullptr)
    {
        m_pData = new char[MAX_DATA_SIZE];
        memset(m_pData, 0, MAX_DATA_SIZE);
    }

    ObjectList::~ObjectList()
    {
        if (nullptr != m_pData)
        {
            delete[] m_pData;
            m_pData = nullptr;
        }
    }

    bool ObjectList::AddObject(const Geometry& geo)
    {
        char* pCurr = reinterpret_cast<char*>(m_pData) + m_size;
        unsigned __int32 gtype = geo.GetType();
        unsigned __int32 gsize = geo.GetSize();

        unsigned __int32 inc = sizeof(gtype) + sizeof(gsize) + gsize;
        if (m_size + inc >= MAX_DATA_SIZE)
        {
            return false;
        }

        m_size += inc;
        m_numObj++;

        memcpy(pCurr, &gtype, sizeof(gtype));
        pCurr += sizeof(gtype);
        memcpy(pCurr, &gsize, sizeof(gsize));
        pCurr += sizeof(gsize);
        memcpy(pCurr, geo.GetData(), gsize);

        return true;
    }
} // namespace AtomTraceCL