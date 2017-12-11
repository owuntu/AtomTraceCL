#define MAX_DEPTH 4
#define PI_F      3.14159265358979323846f
#define HALF_PI_F 1.57079632679f
#define TWO_PI_F  6.28318530718f
#define EPSILON 0.01f

#include "Ray.hcl"
#include "Transformation.hcl"

typedef struct
{
    Transformation transform;
    uint gtype;
    uint gsize;
    uint geometryIndex; // Geometry index from start address of the scene
    uint matIndex;
}ObjectHeader;

typedef struct
{
    Transformation transform;
    uint gtype;
    uint geometryIndex; // Index from start address of the scene
    uint matIndex; // Index from start address of the scene
}HitInfo;

typedef struct
{
    float3 hitP;
    float3 hitN;
}HitInfoGeo; // Work around for an LLVM error. These two should be put into HitInfo

// --- Geometries ----
// TODO: Add triangulate objs.

// -------------------

typedef struct
{
    float3 emission;
    float3 color;
}DiffuseMaterial;

/* This is a description of a RenderObject that store into the scene
struct
{
    ObjectHeader header;
    struct
    {
        geometry data
    };
    DiffuseMaterial mat;
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
    ray.dir = tar - orig;
    //ray.dir = normalize(ray.dir);
    ray.orig = orig;
    return ray;
}

bool IntersectSphere(const Ray* ray, float* t)
{
    // Assuming ray is transform into this unit sphere's local space,
    // which located at the origin (0,0,0).
    float a = dot(ray->dir, ray->dir);
    float b = 2.f * dot(ray->orig, ray->dir);
    float c = dot(ray->orig, ray->orig) - 1.0f;

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
    float3 d = pRAY->dir;
    float3 pos = pRAY->orig;
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

bool IntersectP(__constant char* pObj, __constant int* pIndexTable, int numObjs, const Ray* pRay, const float maxt, bool bIgnoreLight)
{
    bool bHit = false;
    float t = maxt;
    ObjectHeader objh;
    for (int i = 0; i < numObjs; ++i)
    {
        objh = *(__constant ObjectHeader*)(pObj + pIndexTable[i]);

        if (bIgnoreLight)
        {
            DiffuseMaterial mat = *(__constant DiffuseMaterial*)(pObj + objh.matIndex);
            if (fast_length(mat.emission) > EPSILON)
            {
                continue;
            }
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
    }
    return bHit;
}

bool Intersect(__constant char* pObj, __constant int* pIndexTable, int numObjs, const Ray* pRay,
               HitInfo* pHitInfo, HitInfoGeo* pHitInfoGeo, bool bIgnoreLight)
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

        if (bIgnoreLight)
        {
            DiffuseMaterial mat = *(__constant DiffuseMaterial*)(pObj + objh.matIndex);
            if (fast_length(mat.emission) > EPSILON)
            {
                continue;
            }
        }

        Ray ray = RayTransformTo(&objh.transform, pRay);

        if (objh.gtype == 1) // SPHERE
        {
            tHit = IntersectSphere(&ray, &t);

            hitP = ray.orig + ray.dir * t;
            hitN = normalize(hitP);

            hitP = TransformFrom(&objh.transform, &hitP);
            hitN = normalize(VectorTransformFrom(&objh.transform, &hitN));
        }
        else if (objh.gtype == 2) // PLANE
        {
            tHit = IntersectPlane(&ray, &t);

            hitP = ray.orig + ray.dir * t;
            hitP = TransformFrom(&objh.transform, &hitP);

            float3 unitY = (float3)(0.f, 1.f, 0.f);
            hitN = normalize(VectorTransformFrom(&objh.transform, &unitY));
        }
        
        if (tHit)
        {
            bHit = true;
            pHitInfo->transform = objh.transform;
            pHitInfo->gtype = objh.gtype;
            pHitInfo->geometryIndex = objh.geometryIndex;
            pHitInfo->matIndex = objh.matIndex;
            pHitInfoGeo->hitP = hitP;
            pHitInfoGeo->hitN = hitN;
        }
    }
    return bHit;
}

void SampleLights(__constant char* pObjs, __constant int* pIndexTable, int numObjs,
                  const float3* pHitP, const float3* pN, float3* result, uint* pSeed0, uint* pSeed1)
{
    for(int i = 0; i < numObjs; ++i)
    {
        ObjectHeader objh = *(__constant ObjectHeader*)(pObjs + pIndexTable[i]);

        DiffuseMaterial mat = *(__constant DiffuseMaterial*)(pObjs + objh.matIndex);
        if (fast_length(mat.emission) > 0.1f)
        {
            float3 sampP = (float3)(0.f);
            if (objh.gtype == 1) // SPHERE
            {
                // Sample a point on the surface that is facing the shading object.
                float u1 = GetRandom01(pSeed0, pSeed1);
                float u2 = GetRandom01(pSeed0, pSeed1);

                float3 az = TransformTo(&objh.transform, pHitP);
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
                sampP = TransformFrom(&objh.transform, &sampP);
            }
            else
            {
                // TODO: For now only support sphere shape light.
                continue;
            }

            // cast shadow ray
            Ray shadowRay;
            shadowRay.orig = *pHitP;
            shadowRay.dir = sampP - shadowRay.orig;
            float3 nd = normalize(shadowRay.dir);
            float dnd = dot(*pN, nd);
            shadowRay.orig += nd * EPSILON * dnd;
            if (!IntersectP(pObjs, pIndexTable, numObjs, &shadowRay, 1.0f-EPSILON, true))
            {
                // add light contribution
                *result += mat.emission * max(0.0f, dnd);
            }            
        }
    }

}

float3 Radiance(const Ray* ray, __constant char* pObjs, __constant int* pIndexTable, int numObjs, uint* pSeed0, uint* pSeed1)
{
    Ray currentRay;
    currentRay.orig = ray->orig;
    currentRay.dir = ray->dir;
    int d = 0;
    float3 throughput = (float3)(1.f);
    float3 rad = (float3)(0.f);

    while(d < MAX_DEPTH)
    {
        float t = FLT_MAX;
        HitInfo hInfo;
        HitInfoGeo hInfoGeo;
        bool bHit = Intersect(pObjs, pIndexTable, numObjs, &currentRay, &hInfo, &hInfoGeo, false);

        if (bHit)
        {
            DiffuseMaterial mat = *(__constant DiffuseMaterial*)(pObjs + hInfo.matIndex);
            if (fast_length(mat.emission) > EPSILON)
            {
                rad += mat.emission;
                break;
            }

            float3 hitP = hInfoGeo.hitP;
            float3 hitN = hInfoGeo.hitN;

            // Ignore backface hit
            if (dot(hitN, currentRay.dir) > 0.f)
            {
                break;
            }

            // Direct illumination
            float3 Ld = (float3)(0.f);
            SampleLights(pObjs, pIndexTable, numObjs, &hitP, &hitN, &Ld, pSeed0, pSeed1);
            throughput *= mat.color;
            Ld *= throughput;
            rad += Ld;

            // Indirect illumination
            float3 newDir = hitN;
            SampleHemiSphere(
                GetRandom01(pSeed0, pSeed1), GetRandom01(pSeed0, pSeed1),
                &hitN, &newDir);
            currentRay.orig = hitP + hitN * EPSILON;
            currentRay.dir = newDir;
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
                           __global uint* pSeeds, __global float* color, const uint currentSample)
{
    int pid = get_global_id(0);
    int worksize = width*height;
    if (pid < width * height)
    {
        int px = pid % width;
        int py = pid / width;

        uint seed0 = pSeeds[pid];
        uint seed1 = pSeeds[worksize + pid];

        Ray cRay = CastCamRay(px, py, cam, currentSample);

        float3 rad = Radiance(&cRay, pObjs, pIndexTable, numObjs, &seed0, &seed1);
        float invNs = 1.0f / (currentSample + 1);
        float sNs = (float)currentSample * invNs;
        color[pid * 3]     = sNs * color[pid*3    ] + rad.x * invNs;
        color[pid * 3 + 1] = sNs * color[pid*3 + 1] + rad.y * invNs;
        color[pid * 3 + 2] = sNs * color[pid*3 + 2] + rad.z * invNs;

        pOutput[pid * 3] =      clamp(color[pid*3],      0.f, 1.0f) * 255;
        pOutput[pid * 3 + 1 ] = clamp(color[pid*3 + 1 ], 0.f, 1.0f) * 255;
        pOutput[pid * 3 + 2 ] = clamp(color[pid*3 + 2 ], 0.f, 1.0f) * 255;

        pSeeds[pid] = seed0;
        pSeeds[worksize + pid] = seed1;
    }
}
