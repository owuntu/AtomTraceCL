#ifndef ATOMTRACECL_OBJ_ATTRIBUTE_H_
#define ATOMTRACECL_OBJ_ATTRIBUTE_H_

namespace AtomTraceCL
{
    class ObjectAttribute
    {
    public:
        virtual __int32 GetType() const = 0;
        virtual unsigned __int32 GetSize() const = 0;
        virtual const void* GetData() const = 0;
#if 0
        virtual void* GetData()
        {
            return const_cast<void*>(
                reinterpret_cast<const ObjectAttribute*>(this)->GetData()
                );
        }
#endif
    };
} // namespace AtomTraceCL

#endif // ATOMTRACECL_OBJ_ATTRIBUTE_H_
