#include <cstring>

// AtomMathCL
#include "MathConstant.h"
#include "Vector3.h"

#include "Transformation.h"

namespace AtomTraceCL
{
    using namespace AtomMathCL;

    Transformation::Transformation()
    {
        mat[0] = mat[4] = mat[8] = 1.f;
    }

    void Transformation::Translate(const float tx, const float ty, const float tz)
    {
        pos += Vector3(tx, ty, tz);
    }

    void Transformation::Translate(const AtomMathCL::Vector3& trans)
    {
        pos += trans;
    }

    void Transformation::Scale(const float sx, const float sy, const float sz)
    {
        this->Scale(Vector3(sx, sy, sz));
    }

    void Transformation::Scale(const float scale)
    {
        this->Scale(scale, scale, scale);
    }

    void Transformation::Scale(const AtomMathCL::Vector3& scale)
    {
        Matrix3 smat;
        smat.SetScale(scale);
        this->Transform(smat);
    }

    void Transformation::Rotate(const float degree, const AtomMathCL::Vector3& axis)
    {
        static const float factor = M_PI_F / 180.f;
        Matrix3 rmat;
        rmat.SetRotation(degree * factor, axis);
        this->Transform(rmat);
    }

    void Transformation::Transform(const AtomMathCL::Matrix3& m)
    {
        mat = mat*m;
        pos = m*pos;
    }

    void Transformation::PackTransformation(TransformPackBuffer& buffer) const
    {
        for (int i = 0; i < 9; ++i)
        {
            buffer.data[i] = mat[i];
        }

        buffer.data[9] = pos.X();
        buffer.data[10] = pos.Y();
        buffer.data[11] = pos.Z();

        mat.GetInverse(buffer.itm);
    }

} // namespace AtomTraceCL
