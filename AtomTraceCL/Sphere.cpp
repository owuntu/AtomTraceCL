#include "Ray.h"
#include "Sphere.h"

namespace AtomTraceCL
{
    using namespace AtomMathCL;

    Sphere::Sphere() :
        m_orig(Vector3::ZERO),
        m_radius(0.0f),
        m_emission(Vector3::ZERO),
        m_color(1.0f, 1.0f, 1.0f)
    {
    }

    Sphere::Sphere(float r, const AtomMathCL::Vector3& orig) :
        m_orig(orig),
        m_radius(r),
        m_emission(Vector3::ZERO),
        m_color(1.0f, 1.0f, 1.0f)
    {
    }

    unsigned __int32 Sphere::GetSize() const
    {
        return sizeof(Sphere) - sizeof(Geometry);
    }

    GeometryType Sphere::GetType() const
    {
        return SPHERE;
    }

    const void* Sphere::GetData() const
    {
        return (&m_radius);
    }

    void Sphere::SetEmission(const AtomMathCL::Vector3& em)
    {
        m_emission = em;
    }

    void Sphere::SetColor(const AtomMathCL::Vector3& c)
    {
        m_color = c;
    }

    bool Sphere::Intersect(const Ray& ray)
    {
        Vector3 p = m_orig - ray.m_orig;
        float a = 1.0f;
        float b = -2.f * (p * ray.m_dir);
        float c = p.LengthSq() - m_radius * m_radius;

        float h = b*b - 4.f * a * c;

        if (h < 0.0f) return false;

        return true;
    }

} // namespace AtomTraceCL
