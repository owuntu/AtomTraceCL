#ifndef ATOMTRACECL_TRANSFORMATION_H_
#define ATOMTRACECL_TRANSFORMATION_H_

#include "Matrix3.h"
#include "Vector3.h"

namespace AtomTraceCL
{
    struct TransformPackBuffer
    {
        float data[12];
        AtomMathCL::Matrix3 itm;
    };

    class Transformation
    {
    public:
        /*
        xx, xy, xz, 
        yx, yy, yz, 
        zx, zy, zz,

        0,  1,  2,
        3,  4,  5,
        6,  7,  8
        */

        AtomMathCL::Matrix3 mat;
        AtomMathCL::Vector3 pos;

        Transformation();

        void Translate(const AtomMathCL::Vector3& trans);
        void Translate(const float tx, const float ty, const float tz);
        void Scale(const AtomMathCL::Vector3& scale);
        void Scale(const float sx, const float sy, const float sz);
        void Scale(const float scale);
        void Rotate(const float degree, const AtomMathCL::Vector3& axis);

        void Transform(const AtomMathCL::Matrix3& m);

        void PackTransformation(TransformPackBuffer& buffer) const;
    };
} // namespace AtomTraceCL

#endif // ATOMTRACECL_TRANSFORMATION_H_