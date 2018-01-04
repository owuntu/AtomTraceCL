#ifndef ATOMTRACECL_TRIANGLES_HCL_
#define ATOMTRACECL_TRIANGLES_HCL_

#include "ConstantDef.hcl"
#include "InfoDef.hcl"

typedef struct
{
    // bvh data
    ArrayInfo nodes;
    ArrayInfo elements;
    // triangle geometry data
    ArrayInfo vertices;
    ArrayInfo vts;
    ArrayInfo vns;
    ArrayInfo faces;
    ArrayInfo fts;
    ArrayInfo fns;
}TriMeshHeader;

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

    float nDd = dot(vN, pRAY->dir);
    float t = dot(vN, v0 - pRAY->orig) / nDd;
    if (t < 0.f || t > *pt)
        return false;

    float3 hp = pRAY->orig + pRAY->dir * t;
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

bool IntersectBox(const Ray* pRAY, const Box* pBox, float* pt)
{
    // TODO: Ray-box intersection.
    float tEnter, tExit;
    float3 minp, maxp;
    minp.x = pBox->b[0] - pRAY->orig.x;
    minp.y = pBox->b[1] - pRAY->orig.y;
    minp.z = pBox->b[2] - pRAY->orig.z;

    maxp.x = pBox->b[3] - pRAY->orig.x;
    maxp.y = pBox->b[4] - pRAY->orig.y;
    maxp.z = pBox->b[5] - pRAY->orig.z;

    float tmin[3];
    float tmax[3];

    for (int i = 0; i < 3; ++i)
    {
        // Check parallel
        if (pRAY->dir[i] < EPSILON && pRAY->dir[i] > -EPSILON)
        {
            tmin[i] = -FLT_MAX;
            tmax[i] = FLT_MAX;
            continue;
        }

        tmin[i] = minp[i] / pRAY->dir[i];
        tmax[i] = maxp[i] / pRAY->dir[i];
    }

    tEnter = max(tmin[0], max(tmin[1], tmin[2]));
    tExit =  min(tmax[0], min(tmax[1], tmax[2]));

    if (tEnter > tExit)
        return false;
    return true;
}

#endif // ATOMTRACECL_TRIANGLES_HCL_
