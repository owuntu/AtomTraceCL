#ifndef ATOMTRACECL_TRIANGLES_HCL_
#define ATOMTRACECL_TRIANGLES_HCL_

#include "..\cyBVHMacro.h"

#include "ConstantDef.hcl"
#include "InfoDef.hcl"
#include "..\TriMeshHeader.h"

// Axis aligned box
typedef struct
{
    float b[6];
}Box;

typedef struct
{
    Box box;
    uint data;
}BVHNode;

const float3 BaryCentricHelp(const float3 v01, const float3 v02, const float3 v0p, const float3 faceN)
{
    float a0 = dot(cross(v01, v02), faceN);
    float a1 = dot(cross(v01, v0p), faceN);
    float a2 = dot(cross(v0p, v02), faceN);

    float3 bc;
    bc.x = a1 / a0;
    bc.y = a2 / a0;
    bc.z = 1 - bc.x - bc.y;
    return bc;
}

bool IntersectTriangle(const Ray* pRAY, __constant float3* pVertices, __constant float3* pNormal,
    __constant uint3* pFaces, __constant uint3* pFaceN, uint faceID, HitInfoGeo* pInfoGeo, float* pt)
{
    uint3 face = *(pFaces + faceID);
    float3 v0 = *(pVertices + face.x);

    float3 v01;
    float3 v02;
    {
        float3 v1 = *(pVertices + face.y);
        float3 v2 = *(pVertices + face.z);
        v01 = v1 - v0;
        v02 = v2 - v0;
    }

    float3 vN = cross(v01, v02);

    float3 vn0 = *(pNormal + (*(pFaceN + faceID)).x);
    if (dot(vn0, vN) < 0.f)
        vN = -vN;

    vN = normalize(vN);

    float nDd = dot(vN, pRAY->m_dir);
    float t = dot(vN, v0 - pRAY->m_orig) / nDd;
    if (t < 0.f || t > *pt)
        return false;

    float3 hp = pRAY->m_orig + pRAY->m_dir * t;
    float3 v0p = hp - v0;
    float3 bc = BaryCentricHelp(v01, v02, v0p, vN);
    if (bc.x < 0.f || bc.y < 0.f || bc.z < 0.f)
        return false;

    *pt = t;
    float3 vn1 = *(pNormal + (*(pFaceN + faceID)).y);
    float3 vn2 = *(pNormal + (*(pFaceN + faceID)).z);
    pInfoGeo->hitP = hp;
    pInfoGeo->hitN = vn0 * bc.z + vn1 * bc.y + vn2 * bc.x;
    return true;
}

bool IntersectBox(const Ray* pRAY, const Box* pBox, float t)
{
    float tmin = -FLT_MAX;
    float tmax = FLT_MAX;

    for (int i = 0; i < 3; ++i)
    {
        // Check parallel
        if (pRAY->m_dir[i] != 0.f)
        {
            float t1 = (pBox->b[i] - pRAY->m_orig[i]) / pRAY->m_dir[i];
            float t2 = (pBox->b[i + 3] - pRAY->m_orig[i]) / pRAY->m_dir[i];
            tmin = max(tmin, min(t1, t2));
            tmax = min(tmax, max(t1, t2));
        }
    }

    tmin = max(tmin, 0.f);

    if (tmin > tmax || tmin > t)
        return false;
    return true;
}

bool BVHisLeafNode(__constant const BVHNode* pROOT, uint nodeID)
{
    uint data = (pROOT + nodeID)->data;
    return (data&_CY_BVH_LEAF_BIT_MASK) > 0;
}

uint BVHgetNodeElementCount(__constant const BVHNode* pROOT, uint nodeID)
{
    uint data = (pROOT + nodeID)->data;
    return ((data >> _CY_BVH_ELEMENT_OFFSET_BITS) & _CY_BVH_ELEMENT_COUNT_MASK);
}

uint BVHgetNodeElementsOffset(__constant const BVHNode* pROOT, uint nodeID)
{
    uint data = (pROOT + nodeID)->data;
    return (data&_CY_BVH_ELEMENT_OFFSET_MASK);
}

#endif // ATOMTRACECL_TRIANGLES_HCL_
