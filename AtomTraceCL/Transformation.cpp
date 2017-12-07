#include <cstring>

// AtomMathCL
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

    void Transformation::Transform(const AtomMathCL::Matrix3& m)
    {
        mat = mat*m;
        pos = m*pos;
    }

} // namespace AtomTraceCL
