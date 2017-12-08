#define MAX_DEPTH 6
#define PI_F      3.14159265358979323846f
#define HALF_PI_F 1.57079632679f
#define TWO_PI_F  6.28318530718f
#define EPSILON 0.01f


typedef struct
{
    float tm[12]; // Scale, Rotation, and translation
    float itm[12]; // Only inverse matrix3 of scale and rotation.
}Transformation;

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
    float3 orig;
    float3 dir;
}Ray;

// --- Geometries ----
typedef struct
{
    float radius;
    float3 pos;
}Sphere;

typedef struct
{
    float3 pos;
    float3 norm;
}Plane;

// -------------------

typedef struct
{
    float3 emission;
    float3 color;
}DiffuseMaterial;

/* This is a description of a RenderObject that store into the scene
struct
{
    uint gtype;
    uint gsize;
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
    float3 dir = *pN * cos(theta) + ax * r * cos(phi) + ay * r * sin(phi);
    *p = normalize(dir);
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
    ray.dir = normalize(ray.dir);
    ray.orig = orig;
    return ray;
}

bool IntersectSphere(const Sphere* obj, const Ray* ray, float* t)
{
    bool res = false;
    float3 po = obj->pos - ray->orig;
    float b = dot(po, ray->dir);
    float det = b*b - dot(po, po) + obj->radius * obj->radius;
    if (det < 0.0f)
    {
        return false;
    }
    else
    {
        float eps = 1e-4;
        float lt = *t;
        det = sqrt(det);
        lt = b - det;
        if (lt <= eps)
        {
            lt = b + det;
            if (lt <= eps)
            {
                lt = 0.0;
                return false;
            }
        }

        if (lt < *t)
        {
            *t = lt;
            res = true;
        }
    }
    return res;
}

bool IntersectPlane(const Plane* pOBJ, const Ray* pRAY, float* pt)
{
    // intersect if(dot(pRAY->orig + pRAY->dir * t - pOBJ->pos, pOBJ->norm) == 0)
    float3 d = pRAY->dir;
    float3 n = pOBJ->norm;
    float dn = dot(d, n);
    if (fabs(dn) < EPSILON) // ray is parallel to the plane
    {
        return false;
    }

    float3 op = pOBJ->pos - pRAY->orig;
    float t = dot(op, n) / dn;

    if (t < 0.f)
    {
        // ignore if intersection point is beind the ray.
        return false;
    }

    if (t >= *pt)
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

        if (objh.gtype == 1) // SPHERE
        {
            Sphere sp = *(__constant Sphere*)(pObj + objh.geometryIndex);
            bHit |= IntersectSphere(&sp, pRay, &t);
        }
        else if (objh.gtype == 2) // PLANE
        {
            Plane pl = *(__constant Plane*)(pObj + objh.geometryIndex);
            bHit |= IntersectPlane(&pl, pRay, &t);
        }
    }
    return bHit;
}

bool Intersect(__constant char* pObj, __constant int* pIndexTable, int numObjs, const Ray* pRay,
               float* pt, HitInfo* pHitInfo, bool bIgnoreLight)
{
    bool bHit = false;
    bool tHit = false;
    ObjectHeader objh;
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

        if (objh.gtype == 1) // SPHERE
        {
            Sphere sp = *(__constant Sphere*)(pObj + objh.geometryIndex);
            tHit = IntersectSphere(&sp, pRay, pt);
        }
        else if (objh.gtype == 2) // PLANE
        {
            Plane pl = *(__constant Plane*)(pObj + objh.geometryIndex);
            tHit = IntersectPlane(&pl, pRay, pt);
        }
        
        if (tHit)
        {
            bHit = true;
            pHitInfo->transform = objh.transform;
            pHitInfo->gtype = objh.gtype;
            pHitInfo->geometryIndex = objh.geometryIndex;
            pHitInfo->matIndex = objh.matIndex;
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
                Sphere splight = *(__constant Sphere*)(pObjs + objh.geometryIndex);

                // Sample a point on the surface that is facing the shading object.
                float u1 = GetRandom01(pSeed0, pSeed1);
                float u2 = GetRandom01(pSeed0, pSeed1);

                float3 az = *pHitP - splight.pos;
                float maxTheta = acos(splight.radius / length(az));
                az = normalize(az);

                float3 ax = UNIT_X;
                if (1.f - fabs(dot(az, ax)) <= EPSILON)
                {
                    ax = UNIT_Y;
                }
                float3 ay = cross(az, ax);
                ax = cross(ay, az);

                float theta = maxTheta * u1;
                float phi = TWO_PI_F * u2;
                float r = sin(theta);
                sampP = ax * r * cos(phi) + ay * r * sin(phi) + az * cos(theta);
                sampP = splight.pos + normalize(sampP) * splight.radius;
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
            float len = length(shadowRay.dir);
            shadowRay.dir = normalize(shadowRay.dir);
            float dnd = dot(*pN, shadowRay.dir);
            shadowRay.orig += shadowRay.dir * EPSILON * dnd;
            if (!IntersectP(pObjs, pIndexTable, numObjs, &shadowRay, len, true))
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
        bool bHit = Intersect(pObjs, pIndexTable, numObjs, &currentRay, &t, &hInfo, false);

        if (bHit)
        {
            DiffuseMaterial mat = *(__constant DiffuseMaterial*)(pObjs + hInfo.matIndex);
            if (fast_length(mat.emission) > EPSILON)
            {
                rad += mat.emission;
                break;
            }

            float3 hitP = currentRay.orig + currentRay.dir * t;
            float3 hitN;
            {
                __constant char* pGeo = pObjs + hInfo.geometryIndex;
                if (hInfo.gtype == 1) // SPHERE
                {
                    Sphere sHit = *(__constant Sphere*)(pGeo);
                    hitN = hitP - sHit.pos;
                }
                else if (hInfo.gtype == 2) // PLANE
                {
                    Plane plHit = *(__constant Plane*)(pGeo);
                    hitN = plHit.norm;
                }
            }
            hitN = normalize(hitN);

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
