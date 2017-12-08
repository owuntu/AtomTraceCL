#ifndef ATOMTRACECL_TRANSFORMATION_HCL_
#define ATOMTRACECL_TRANSFORMATION_HCL_

typedef struct
{
    float tm[12]; // Scale, Rotation, and translation
    float itm[12]; // Only inverse matrix3 of scale and rotation.
}Transformation;

void GetTranslation(const Transformation* pTf, float3* pPos)
{
    pPos->x = pTf->tm[9];
    pPos->y = pTf->tm[10];
    pPos->z = pTf->tm[11];
}

float3 Multiply(const float* pMat3, const float3* pVec)
{
    float3 res;
    res.x = pVec->x * pMat3[0] + pVec->y * pMat3[3] + pVec->z * pMat3[6];
    res.y = pVec->x * pMat3[1] + pVec->y * pMat3[4] + pVec->z * pMat3[7];
    res.z = pVec->x * pMat3[2] + pVec->y * pMat3[5] + pVec->z * pMat3[8];
    return res;
}

void ApplyTransform(const float* pMat3, float3* pVec)
{
    float3 res;
    res.x = pVec->x * pMat3[0] + pVec->y * pMat3[3] + pVec->z * pMat3[6];
    res.y = pVec->x * pMat3[1] + pVec->y * pMat3[4] + pVec->z * pMat3[7];
    res.z = pVec->x * pMat3[2] + pVec->y * pMat3[5] + pVec->z * pMat3[8];
    *pVec = res;
}

// Transform to local coordinate
float3 TransformTo(const Transformation* pTf, const float3* pPoint)
{
    float3 pos;
    GetTranslation(pTf, &pos);
    pos = *pPoint - pos;
    ApplyTransform(pTf->itm, &pos);
    return pos;
}

// Transform from local coordinate to world coordinate
float3 TransformFrom(const Transformation* pTf, const float3* pPoint)
{
    float3 pos = *pPoint;
    float3 trans;
    GetTranslation(pTf, &trans);
    ApplyTransform(pTf->tm, &pos);
    pos += trans;
    return pos;
}

float3 TransposeMul(const float* pMat3, const float3* pVec)
{
    float3 res;
    res.x = pMat3[0] * pVec->x + pMat3[1] * pVec->y + pMat3[2] * pVec->z;
    res.y = pMat3[3] * pVec->x + pMat3[4] * pVec->y + pMat3[5] * pVec->z;
    res.z = pMat3[6] * pVec->x + pMat3[7] * pVec->y + pMat3[8] * pVec->z;
    return res;
}

float3 VectorTransformTo(const Transformation* pTf, const float3* pDir)
{
    return TransposeMul(pTf->tm, pDir);
}

float3 VectorTransfromFrom(const Transformation* pTf, const float3* pDir)
{
    return TransposeMul(pTf->itm, pDir);
}

#endif // ATOMTRACECL_TRANSFORMATION_HCL_
