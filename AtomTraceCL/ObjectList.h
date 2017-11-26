#ifndef ATOMTRACECL_OBJLIST_H_
#define ATOMTRACECL_OBJLIST_H_


namespace AtomTraceCL
{
    typedef struct
    {
        unsigned __int32 m_size; // size include m_size, m_type, and all data point from m_pData
        __int32 m_type;
        void* m_pData;
    }Object;

    class ObjectList
    {
    public:
        unsigned __int32 m_size;
        unsigned __int32 m_numObj;
        void* m_pData;

    public:
        void LoadScene();
    };

} // namespace AtomTraceCL

#endif // ATOMTRACECL_OBJLIST_H_