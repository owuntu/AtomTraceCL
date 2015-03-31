#include <iostream>

#include "Vector3.h"

namespace AtomMathCL
{
    const Vector3 Vector3::ZERO  (0.0f, 0.0f, 0.0f);
    const Vector3 Vector3::UNIT_X(1.0f, 0.0f, 0.0f);
    const Vector3 Vector3::UNIT_Y(0.0f, 1.0f, 0.0f);
    const Vector3 Vector3::UNIT_Z(0.0f, 0.0f, 1.0f);


    const cl_float Vector3::operator*(const Vector3& rhs) const
    {
        cl_float res = m_x * rhs.m_x + m_y * rhs.m_y + m_z * rhs.m_z;
        return res;
    }

    const Vector3 Vector3::operator*(const cl_float s) const
    {
        Vector3 res(m_x*s, m_y*s, m_z*s);
        return res;
    }

    const Vector3 Vector3::operator-(const Vector3& rhs) const
    {
        Vector3 v;
        v.m_x = m_x - rhs.m_x;
        v.m_y = m_y - rhs.m_y;
        v.m_z = m_z - rhs.m_z;
        return v;
    }

}