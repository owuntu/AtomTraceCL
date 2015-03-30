#include "Ray.h"

using namespace AtomMathCL;

Ray::Ray() :
    m_orig(Vector3::ZERO),
    m_dir(Vector3::UNIT_X)
{}