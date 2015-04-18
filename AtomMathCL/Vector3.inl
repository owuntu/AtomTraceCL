#include <iostream>
#include "Vector3.h"

namespace AtomMathCL
{
    Vector3::Vector3() :
        m_x(0.0f),
        m_y(0.0f),
        m_z(0.0f)
    {}

    Vector3::Vector3(cl_float s) :
        m_x(s),
        m_y(s),
        m_z(s)
    {}

    Vector3::Vector3(cl_float x, cl_float y, cl_float z) :
        m_x(x),
        m_y(y),
        m_z(z)
    {}

    Vector3::Vector3(const Vector3& other) :
        m_x(other.m_x),
        m_y(other.m_y),
        m_z(other.m_z)
    {}

    Vector3::~Vector3()
    {}

    const cl_float Vector3::LengthSq() const
    {
        return (m_x*m_x + m_y*m_y + m_z*m_z);
    }

    // Accessors
    const cl_float Vector3::X() const
    {
        return m_x;
    }

    const cl_float Vector3::Y() const
    {
        return m_y;
    }

    const cl_float Vector3::Z() const
    {
        return m_z;
    }

    const cl_float& Vector3::operator[](const std::size_t& index) const
    {
        if (index >= 3)
        {
            std::cerr << "Error in Vector3::operator[]: index exceed 3.\n";
        }
        return (&m_x)[index];
    }

    cl_float& Vector3::operator[](const std::size_t& index)
    {
        return const_cast<cl_float&>
            (
            (static_cast<const Vector3*>(this))->
            operator[](index)
            );
    }

    const Vector3& Vector3::operator=(const Vector3& rhs)
    {
        m_x = rhs.m_x;
        m_y = rhs.m_y;
        m_z = rhs.m_z;
        return *this;
    }

    void Vector3::X(const cl_float in_x)
    {
        m_x = in_x;
    }

    void Vector3::Y(const cl_float in_y)
    {
        m_y = in_y;
    }

    void Vector3::Z(const cl_float in_z)
    {
        m_z = in_z;
    }

    void Vector3::Set(const cl_float i_x, const cl_float i_y, const cl_float i_z)
    {
        m_x = i_x;
        m_y = i_y;
        m_z = i_z;
    }

}