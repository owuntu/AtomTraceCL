#define MAX_DEPTH 3

#include "ConstantDef.hcl"
#include "InfoDef.hcl"
#include "Metal.hcl"
#include "Ray.hcl"
#include "Transformation.hcl"
#include "Triangles.hcl"

// --- Geometries ----
// TODO: Add triangulate objs.

// -------------------

typedef struct
{
    float3 color;
}DiffuseMaterial;

typedef struct
{
    float3 emission;
}Light;

/* This is a description of a RenderObject that store into the scene
struct
{
    ObjectHeader header;
    struct
    {
        geometry data
    };
    struct
    {
        material data
    };

}RenderObject;
*/

typedef struct
{
    float3 pos;
    float3 upLeftPix;
    float3 dx; // camera right direction, per-pixel width
    float3 dy; // camera up direction, per-pixel height
}Camera;

__constant float3 UNIT_X = (float3)(1.f, 0.f, 0.f);
__constant float3 UNIT_Y = (float3)(0.f, 1.f, 0.f);
__constant float3 UNIT_Z = (float3)(0.f, 0.f, 1.f);

// This function is copy from: http://davibu.interfree.it/opencl/smallptgpu2/smallptGPU2.html
float GetRandom01(uint *pSeed0, uint *pSeed1) {
    *pSeed0 = 36969 * ((*pSeed0) & 65535) + ((*pSeed0) >> 16);
    *pSeed1 = 18000 * ((*pSeed1) & 65535) + ((*pSeed1) >> 16);

    unsigned int ires = ((*pSeed0) << 16) + (*pSeed1);

    // Convert to float
    union {
        float f;
        unsigned int ui;
    } res;
    res.ui = (ires & 0x007fffff) | 0x40000000;

    return (res.f - 2.f) / 2.f;
}

float Halton(int index, int base)
{
    float r = 0;
    float f = 1.0f / (float)base;
    for (int i = index; i > 0; i /= base)
    {
        r += f * (i%base);
        f /= (float)base;
    }
    return r;
}

void UniformSampleSphere(float u1, float u2, float3* p)
{
    float theta = HALF_PI_F*(1.f - u1 * 2.f); // [-PI/2, PI/2]
    p->z = cos(theta);
    float r = sin(theta);
    float phi = TWO_PI_F * u2;
    p->x = r * cos(phi);
    p->y = r * sin(phi);
}

void SampleHemiSphere(float u1, float u2, const float3* pN, float3* p)
{
    float3 ax;
    if (1.f - fabs(dot(UNIT_X, *pN)) < 0.01f)
    {
        ax = cross(*pN, UNIT_Y);
    }
    else
    {
        ax = cross(*pN, UNIT_X);
    }

    float3 ay = cross(ax, *pN);

    ax = normalize(ax);
    ay = normalize(ay);

    // Importance sampling
    float theta = 0.5f * acos(1.f - 2.f * u1);
    //float theta = HALF_PI_F * u1;
    float phi = TWO_PI_F * u2;
    float r = sin(theta);
    *p = *pN * cos(theta) + ax * r * cos(phi) + ay * r * sin(phi);
}

// fov is the vertial fov angle
Ray CastCamRay(int px, int py, __constant Camera* cam, const uint npp)
{
    float fx = (float)px - 0.5f + Halton((int)npp, 2);
    float fy = (float)py - 0.5f + Halton((int)npp, 3);
    float3 tar = cam->upLeftPix 
                 + cam->dx * fx
                 - cam->dy * fy;

    float3 orig = cam->pos;

    Ray ray;
    ray.m_dir = tar - orig;
    //ray.m_dir = normalize(ray.m_dir);
    ray.m_orig = orig;
    return ray;
}

bool IntersectSphere(const Ray* ray, float* t)
{
    // Assuming ray is transform into this unit sphere's local space,
    // which located at the origin (0,0,0).
    float a = dot(ray->m_dir, ray->m_dir);
    float b = 2.f * dot(ray->m_orig, ray->m_dir);
    float c = dot(ray->m_orig, ray->m_orig) - 1.0f;

    float h = b*b - 4.0f * a * c;
    if (h<0) return false;

    a = 0.5f / a;
    b = -b * a;
    h = sqrt(h) * a;
    float t1 = b - h;
    float t2 = b + h;

    if (t1 < 0.0f && t2 < 0.0f)
        return false;

    float tz;
    if (t1 >= 0.0f)
        tz = t1;
    else tz = t2;

    if (tz < *t) {
        *t = tz;
        return true;
    }

    return false;
}

bool IntersectPlane(const Ray* pRAY, float* pt)
{
    // Assuming ray is transform into plane's local space, which has normal (0,1,0)
    //   located at the origin (0,0,0), with size [(-1,0,-1),(1,0,1)].
    float3 d = pRAY->m_dir;
    float3 pos = pRAY->m_orig;
    if (d.y == 0.f) // ray is parallel to the plane
    {
        return false;
    }

    float t = -pos.y / d.y;
    if (t < 0.f)
    {
        // ignore if intersection point is beind the ray.
        return false;
    }

    if (t >= *pt)
    {
        return false;
    }

    float3 hitP = pos + d *t;
    if (hitP.x < -1.f || hitP.x > 1.f || hitP.z < -1.f || hitP.z > 1.f)
    {
        return false;
    }

    *pt = t;

    return true;
}

bool IntersectP(__constant char* pObj, __constant const int* pIndexTable, int numObjs, const Ray* pRay, const float maxt, bool bIgnoreLight)
{
    bool bHit = false;
    float t = maxt;
    ObjectHeader objh;
    for (int i = 0; i < numObjs; ++i)
    {
        objh = *(__constant ObjectHeader*)(pObj + pIndexTable[i]);

        if (bIgnoreLight && objh.matType == 0) // light type
        {
            continue;
        }

        Ray ray = RayTransformTo(&objh.transform, pRay);

        if (objh.gtype == 1) // SPHERE
        {
            bHit |= IntersectSphere(&ray, &t);
        }
        else if (objh.gtype == 2) // PLANE
        {
            bHit |= IntersectPlane(&ray, &t);
        }
        else if (objh.gtype == 3) // Triangles object
        {
            HitInfoGeo hInfo;
            bHit |= IntersectTriMesh(&ray, pObj + objh.geometryIndex, &hInfo, &t);
        }
    }
    return bHit;
}

bool Intersect(__constant char* pObj, __constant const int* pIndexTable, int numObjs, const Ray* pRay,
               ObjectHeader* pObjHead, HitInfoGeo* pHitInfoGeo, bool bIgnoreLight)
{
    bool bHit = false;
    bool tHit = false;
    ObjectHeader objh;
    float t = FLT_MAX;
    float3 hitP;
    float3 hitN;
    for (int i = 0; i < numObjs; ++i)
    {
        __constant char* pCurr = pObj + pIndexTable[i];

        objh = *(__constant ObjectHeader*)pCurr;

        if (bIgnoreLight && objh.matType == 0) // light type
        {
            continue;
        }

        Ray ray = RayTransformTo(&objh.transform, pRay);

        if (objh.gtype == 1) // SPHERE
        {
            tHit = IntersectSphere(&ray, &t);

            hitP = ray.m_orig + ray.m_dir * t;
            hitN = normalize(hitP);

            hitP = TransformFrom(&objh.transform, &hitP);
            hitN = normalize(VectorTransformFrom(&objh.transform, &hitN));
        }
        else if (objh.gtype == 2) // PLANE
        {
            tHit = IntersectPlane(&ray, &t);

            hitP = ray.m_orig + ray.m_dir * t;
            hitP = TransformFrom(&objh.transform, &hitP);

            float3 uy = (float3)(0.f, 1.f, 0.f);
            hitN = normalize(VectorTransformFrom(&objh.transform, &uy));
        }
        else if (objh.gtype == 3) // Triangles object
        {
            tHit = IntersectTriMesh(&ray, pObj + objh.geometryIndex, pHitInfoGeo, &t);
            // Transform back into world coordinate
            hitP = TransformFrom(&objh.transform, &(pHitInfoGeo->hitP));
            hitN = normalize(VectorTransformFrom(&objh.transform, &(pHitInfoGeo->hitN)));
        }
        
        if (tHit)
        {
            bHit = true;
            *pObjHead = objh;

            pHitInfoGeo->hitP = hitP;
            pHitInfoGeo->hitN = hitN;
        }
    }
    return bHit;
}

void SampleLight(__constant char* pObjs, const ObjectHeader* pObjHead, __constant int* pIndexTable, const int numObjs,
                 const float3* pHitP, const float3* pN, float3* pLd, float3* pWi, float* pCosWi, uint* pSeed0, uint* pSeed1)
{
    DiffuseMaterial light = *(__constant DiffuseMaterial*)(pObjs + pObjHead->matIndex);
    float3 sampP = (float3)(0.f);
    if (pObjHead->gtype == 1) // SPHERE
    {
        // Sample a point on the surface that is facing the shading object.
        float u1 = GetRandom01(pSeed0, pSeed1);
        float u2 = GetRandom01(pSeed0, pSeed1);

        float3 az = TransformTo(&pObjHead->transform, pHitP);
        float maxTheta = acos(1.0f / length(az));
        az = normalize(az);

        float3 ax = UNIT_X;
        if (1.f - fabs(dot(az, ax)) <= EPSILON)
        {
            ax = UNIT_Y;
        }
        float3 ay = cross(az, ax);
        ax = cross(ay, az);
        ax = normalize(ax);
        ay = normalize(ay);

        float theta = maxTheta * u1;
        float phi = TWO_PI_F * u2;
        float r = sin(theta);
        sampP = ax * r * cos(phi) + ay * r * sin(phi) + az * cos(theta);
        //sampP = normalize(sampP);
        sampP = TransformFrom(&pObjHead->transform, &sampP);
    }
    else
    {
        // TODO: For now only support sphere shape light.
        *pLd = (float3)(0.f);
        *pCosWi = 0.f;
        return;
    }

    // cast shadow ray
    Ray shadowRay;
    shadowRay.m_orig = *pHitP;
    shadowRay.m_dir = sampP - shadowRay.m_orig;
    float3 wi = normalize(shadowRay.m_dir);
    float cosWi = dot(*pN, wi);
    if (cosWi == 0.f)
    {
        return;
    }

    shadowRay.m_orig += wi * EPSILON / fabs(cosWi);
    // Shadow check
    if (!IntersectP(pObjs, pIndexTable, numObjs, &shadowRay, 1.0f-EPSILON, true))
    {
        // add light contribution
        //float l2 = length(shadowRay.m_dir);
        //l2 *= l2;
        *pLd = light.color;// / l2;
        *pCosWi = cosWi;
        *pWi = wi;
    }
}

float3 Radiance(const Ray* ray, __constant char* pObjs, __constant int* pIndexTable, int numObjs, uint* pSeed0, uint* pSeed1)
{
    Ray currentRay;
    currentRay.m_orig = ray->m_orig;
    currentRay.m_dir = ray->m_dir;
    int d = 0;
    //float3 throughput = (float3)(1.f);
    float3 rad = (float3)(0.f);
    float3 preCi = (float3)(1.f);
    float cosWi2nd = 1.0f;
    DiffuseMaterial mat;
    Metal metal;
    while(d < MAX_DEPTH)
    {
        float t = FLT_MAX;
        ObjectHeader hInfo;
        HitInfoGeo hInfoGeo;
        bool bHit = Intersect(pObjs, pIndexTable, numObjs, &currentRay, &hInfo, &hInfoGeo, false);

        if (bHit)
        {
            if (hInfo.matType == 0) // light
            {
                mat = *(__constant DiffuseMaterial*)(pObjs + hInfo.matIndex);
                rad += mat.color;
                break;
            }

            float3 hitP = hInfoGeo.hitP;
            float3 hitN = hInfoGeo.hitN;
            float3 wo = normalize(-currentRay.m_dir);
            float cosWo = dot(hitN, wo);

            // Ignore backface hit;
            if (cosWo <= 0.f)
            {
                break;
            }

            if (hInfo.matType == 1) // Diffuse
            {
                mat = *(__constant DiffuseMaterial*)(pObjs + hInfo.matIndex);
            }
            else if (hInfo.matType == 2) // Metal
            {
                metal = *(__constant Metal*)(pObjs + hInfo.matIndex);
            }

            // Direct illumination
            for (int i = 0; i < numObjs; ++i)
            {
                ObjectHeader lightHead = *(__constant ObjectHeader*)(pObjs + pIndexTable[i]);
                if (lightHead.matType == 0) // LIGHT type
                { 
                    float3 Ld = (float3)(0.f);
                    float3 wi = (float3)(0.f);
                    float cosWi = 0.f;
                    SampleLight(pObjs, &lightHead, pIndexTable, numObjs, &hitP, &hitN, &Ld, &wi, &cosWi, pSeed0, pSeed1);

                    if (cosWo * cosWi <= 0.f)
                    {
                        continue;
                    }

                    if (hInfo.matType == 1) // Diffuse
                    {
                        //rad += Ld * fabs(cosWi) * f;
                        rad += Ld * fabs(cosWi) * (preCi * mat.color);
                    }
                    else if (hInfo.matType == 2) // Metal
                    {
                        float3 tf = MetalF(&metal, &wo, &hitN, &wi);
                        rad += Ld * tf * fabs(cosWi) * preCi;

                    }
                }
            }

            // Indirect illumination
            float3 newDir;
            SampleHemiSphere( GetRandom01(pSeed0, pSeed1), GetRandom01(pSeed0, pSeed1), &hitN, &newDir );
            currentRay.m_orig = hitP + hitN * EPSILON;
            currentRay.m_dir = newDir;
            cosWi2nd = dot(newDir, hitN);

            if (hInfo.matType == 1) // Diffuse
            {
                preCi *= cosWi2nd * mat.color;
            }
            else if (hInfo.matType == 2) // Metal
            {
                preCi *= cosWi2nd * MetalF(&metal, &wo, &hitN, &newDir);
            }
        }
        else // miss
        {
            break;
        }
        ++d;
    }
    return rad;
}

__kernel void RenderKernel(__global uchar* pOutput, int width, int height,
                           __constant Camera* cam,
                           __constant char* pObjs, __constant int* pIndexTable, int numObjs,
                           __global uint* pSeeds, __global float* color, uint currentSample)
{
    int pid = get_global_id(0);
    int worksize = width*height;
    if (pid < width * height)
    {
        int px = pid % width;
        int py = pid / width;

        uint seed0 = pSeeds[pid];
        uint seed1 = pSeeds[worksize + pid];

        // This increase magic number should match the one in the host.
        // TODO: Take it from kernel parameter.
        for (int i = 0; i < 64; ++i)
        {
            Ray cRay = CastCamRay(px, py, cam, currentSample);

            float3 rad = Radiance(&cRay, pObjs, pIndexTable, numObjs, &seed0, &seed1);
            float invNs = 1.0f / (currentSample + 1);
            float sNs = (float)currentSample * invNs;
            color[pid * 3] = sNs * color[pid * 3] + rad.x * invNs;
            color[pid * 3 + 1] = sNs * color[pid * 3 + 1] + rad.y * invNs;
            color[pid * 3 + 2] = sNs * color[pid * 3 + 2] + rad.z * invNs;
            currentSample++;
        }
        // Apply Gamma correction
        pOutput[pid * 3] =      clamp(powr(color[pid*3],      1.f / 2.2f), 0.f, 1.0f) * 255;
        pOutput[pid * 3 + 1 ] = clamp(powr(color[pid*3 + 1 ], 1.f / 2.2f), 0.f, 1.0f) * 255;
        pOutput[pid * 3 + 2 ] = clamp(powr(color[pid*3 + 2 ], 1.f / 2.2f), 0.f, 1.0f) * 255;

        pSeeds[pid] = seed0;
        pSeeds[worksize + pid] = seed1;
    }
}
