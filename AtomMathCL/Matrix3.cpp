#include <cstring>

#include "Vector3.h"
#include "Matrix3.h"

namespace AtomMathCL
{
    Matrix3::Matrix3()
    {
        memset(m_data, 0, sizeof(m_data));
    }

    const Matrix3 Matrix3::operator*(const Matrix3& rhs) const
    {
        Matrix3 mat;

        for (int r = 0; r < 3; ++r)
        {
            for (int c = 0; c < 3; ++c)
            {
                mat[r * 3 + c] = 0.f;
                for (int k = 0; k < 3; ++k)
                {
                    mat[r * 3 + c] += m_data[r * 3 + k] * rhs[k * 3 + c];
                }
            }
        }

        return mat;
    }

    const Matrix3 Matrix3::operator+(const Matrix3& rhs) const
    {
        Matrix3 mat;
        for (int i = 0; i < 9; i += 3)
        {
            mat[i]   = m_data[i]   + rhs[i];
            mat[i+1] = m_data[i+1] + rhs[i+1];
            mat[i+2] = m_data[i+2] + rhs[i+2];
        }
        return mat;
    }

    const Matrix3 Matrix3::operator-(const Matrix3& rhs) const
    {
        Matrix3 mat;
        for (int i = 0; i < 9; i += 3)
        {
            mat[i]     = m_data[i]     - rhs[i];
            mat[i + 1] = m_data[i + 1] - rhs[i + 1];
            mat[i + 2] = m_data[i + 2] - rhs[i + 2];
        }
        return mat;
    }

    float& Matrix3::operator[](int index)
    {
        return m_data[index];
    }

    const float Matrix3::operator[](int index) const
    {
        return m_data[index];
    }

    // Helper function
    inline void Swap(float& a, float& b)
    {
        float t = a;
        a = b;
        b = t;
    }

    void Matrix3::Transpose()
    {
        Swap(m_data[1], m_data[3]);
        Swap(m_data[2], m_data[6]);
        Swap(m_data[5], m_data[7]);
    }

    Matrix3 Matrix3::GetTranspose() const
    {
        Matrix3 mat = *this;
        mat.Transpose();
        return mat;
    }

    void Matrix3::Zero()
    {
        memset(m_data, 0, sizeof(m_data));
    }

    void Matrix3::SetIdentity()
    {
        this->Zero();
        m_data[0] = m_data[4] = m_data[8] = 1.0f;
    }

    void Matrix3::SetScale(const Vector3& scale)
    {
        this->Zero();
        m_data[0] = scale.X();
        m_data[4] = scale.Y();
        m_data[8] = scale.Z();
    }

    void Matrix3::SetScale(float scale)
    {
        this->Zero();
        m_data[0] = m_data[4] = m_data[8] = scale;
    }

    /// Set a rotation matrix about the given axis by cos and sin angle theta
    void Matrix3::SetRotation(const Vector3& axis, float c, float s)
    {
        if (c >= 1.0f) {
            SetIdentity();
            return;
        }

        float t = 1 - c;
        float tx = t * axis.X();
        float ty = t * axis.Y();
        float tz = t * axis.Z();
        float txy = tx * axis.Y();
        float txz = tx * axis.Z();
        float tyz = ty * axis.Z();
        float sx = s * axis.X();
        float sy = s * axis.Y();
        float sz = s * axis.Z();

        m_data[0] = tx * axis.X() + c;
        m_data[1] = txy + sz;
        m_data[2] = txz - sy;

        m_data[3] = txy - sz;
        m_data[4] = ty * axis.Y() + c;
        m_data[5] = tyz + sx;

        m_data[6] = txz + sy;
        m_data[7] = tyz - sx;
        m_data[8] = tz * axis.Z() + c;
    }

    void Matrix3::SetRotate(float degree, const Vector3& axis)
    {

    }

    void Matrix3::SetRotateX(float degree)
    {
        this->SetRotate(degree, Vector3::UNIT_X);
    }

    void Matrix3::SetRotateY(float degree)
    {
        this->SetRotate(degree, Vector3::UNIT_Y);
    }

    void Matrix3::SetRotateZ(float degree)
    {
        this->SetRotate(degree, Vector3::UNIT_Z);
    }

} // namespace AtomMathCL
