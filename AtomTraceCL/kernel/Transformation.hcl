#ifndef ATOMTRACECL_TRANSFORMATION_HCL_
#define ATOMTRACECL_TRANSFORMATION_HCL_

typedef struct
{
    float tm[12]; // Scale, Rotation, and translation
    float itm[12]; // Only inverse matrix3 of scale and rotation.
}Transformation;

// Transform to local coordinate
float3 TransformTo(const Transformation* pTf, const float3* pPoint)
{

}

// Transform from local coordinate to world coordinate
float3 TransformFrom(const Transformation* pTf, const float3* pPoint)
{

}

float3 VectorTransformTo(const Transformation* pTf, const float3* pVec)
{

}

float3 VectorTransfromFrom(const Transformation* pTf, const float3* pVec)
{

}

#endif // ATOMTRACECL_TRANSFORMATION_HCL_
