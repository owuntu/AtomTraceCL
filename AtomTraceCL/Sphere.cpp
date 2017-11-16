#include "Ray.h"
#include "Sphere.h"

using namespace AtomMathCL;

Sphere::Sphere() :
    m_orig(Vector3::ZERO),
    m_radius(0.0f)
{

}

Sphere::Sphere(float r, const AtomMathCL::Vector3& orig) :
    m_orig(orig),
    m_radius(r)
{
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