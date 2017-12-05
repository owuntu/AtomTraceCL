#ifndef ATOMTRACECL_GEOMETRY_H_
#define ATOMTRACECL_GEOMETRY_H_

namespace AtomTraceCL
{
    enum GeometryType
    {
        SPHERE = 1,
        TRIANGLES = 2
    };
    class Geometry
    {
    public:
        virtual unsigned __int32 GetSize() const = 0;
        virtual GeometryType GetType() const = 0;
        virtual const void* GetData() const = 0;
#if 0
        virtual void* GetData()
        {
            return const_cast<void*>(
                        reinterpret_cast<const Geometry*>(this)->GetData()
                   );
        }
#endif
    };

} // namespace AtomTraceCL

#endif // ATOMTRACECL_GEOMETRY_H_