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
        void* m_pData;
        std::vector<int> m_indexTable;

    public:
        ObjectList();
        ~ObjectList();

        //void LoadScene();
        bool AddObject(const Geometry&);
        bool AddObject(const RenderObject&);
    };

} // namespace AtomTraceCL

#endif // ATOMTRACECL_OBJLIST_H_