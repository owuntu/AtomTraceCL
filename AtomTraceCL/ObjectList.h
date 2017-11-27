#ifndef ATOMTRACECL_OBJLIST_H_
#define ATOMTRACECL_OBJLIST_H_


namespace AtomTraceCL
{
    class Geometry;
    class Sphere;

    class ObjectList
    {
    public:
        unsigned __int32 m_size;
        unsigned __int32 m_numObj;
        void* m_pData;

    public:
        ObjectList();
        ~ObjectList();

        //void LoadScene();
        bool AddObject(const Geometry&);
        bool AddSphere(const Sphere&);
    };

} // namespace AtomTraceCL

#endif // ATOMTRACECL_OBJLIST_H_