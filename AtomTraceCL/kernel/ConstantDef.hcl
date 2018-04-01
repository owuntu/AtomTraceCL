#ifndef ATOMTRACECL_CONSTANT_DEF_HCL_
#define ATOMTRACECL_CONSTANT_DEF_HCL_

#define PI_F      3.14159265358979323846f
#define HALF_PI_F 1.57079632679f
#define TWO_PI_F  6.28318530718f
#define INV_PI_F  0.31830988618f
#define EPSILON 0.001f

__constant float3 UNIT_X = (float3)(1.f, 0.f, 0.f);
__constant float3 UNIT_Y = (float3)(0.f, 1.f, 0.f);
__constant float3 UNIT_Z = (float3)(0.f, 0.f, 1.f);

#endif //ATOMTRACECL_CONSTANT_DEF_HCL_
