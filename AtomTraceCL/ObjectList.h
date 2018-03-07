#ifndef ATOMTRACECL_OBJLIST_H_
#define ATOMTRACECL_OBJLIST_H_

#include <vector>

namespace AtomTraceCL
{
    class Geometry;
    class RenderObject;

    class ObjectList
    {
    public:
        unsigned __int32 m_size;
        unsigned __int32 m_numObj;
        void* m_pData; // buffer on host memory to temporary store objects data
        std::vector<int> m_indexTable;

    public:
        ObjectList(unsigned long maxDataSize = 0xFFFFFFF0);
        ~ObjectList();

        //void LoadScene();
        bool AddObject(const RenderObject&);
    private:
        const unsigned long m_MAX_DATA_SIZE; // Maximum buffer to load object, size in byte
    };

} // namespace AtomTraceCL

#endif // ATOMTRACECL_OBJLIST_H_