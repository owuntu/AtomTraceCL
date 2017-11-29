#define MAX_DEPTH 1
#define PI_F 3.14159265358979323846f

typedef struct
{
    float3 orig;
    float3 dir;
}Ray;

typedef struct
{
    float radius;
    float3 pos;
    float3 emission;
    float3 color;
}Sphere;

typedef struct
{
    uint size;
}Object;

typedef struct
{
    float3 pos;
    float3 upLeftPix;
    float3 dx; // camera right direction, per-pixel width
    float3 dy; // camera up direction, per-pixel height
}Camera;

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

void UniformSampleSphere(float u1, float u2, float3* p)
{
    // TODO: apply importance sampling
    float theta = 0.5f * PI_F*(1.f - u1 * 2.f);
    p->z = sin(theta);
    float r = cos(theta);
    float phi = 2.0f * PI_F * u2;
    p->x = r * cos(phi);
    p->y = r * sin(phi);
}

// fov is the vertial fov angle
Ray CastCamRay(int px, int py, __constant Camera* cam, uint* pSeed0, uint* pSeed1)
{
    float fx = (float)px;
    float fy = (float)py;
    float3 tar = cam->upLeftPix 
                 + cam->dx * (fx - 0.5f + GetRandom01(pSeed0, pSeed1))
                 - cam->dy * (fy - 0.5f + GetRandom01(pSeed0, pSeed1));

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

bool IntersectP(__constant char* pObj, int numObjs, const Ray* pRay, const float maxt)
{
    __constant char* pCurr = pObj;
    bool bHit = false;
    float t = maxt;
    for (int i = 0; i < numObjs; ++i)
    {
        uint gtype = *((__constant uint*)pCurr);
        pCurr += sizeof(uint);
        uint gsize = *((__constant uint*)pCurr);
        pCurr += sizeof(uint);

        if (gtype == 1) // SPHERE
        {
            Sphere sp = *(__constant Sphere*)pCurr;
            pCurr += gsize;
            bHit |= IntersectSphere(&sp, pRay, &t);
        }
    }
    return bHit;
}

void SampleLights(__constant char* pObj, int numObjs,
                  const float3* pHitP, const float3* pN, float3* result, uint* pSeed0, uint* pSeed1)
{
    __constant char* pCurr = pObj;
    for(int i = 0; i < numObjs; ++i)
    {
        uint gtype = *((__constant uint*)pCurr);
        pCurr += sizeof(uint);
        uint gsize = *((__constant uint*)pCurr);
        pCurr += sizeof(uint);

        if (gtype == 1) // SPHERE
        {
            Sphere light = *(__constant Sphere*)pCurr;
            if (fast_length(light.emission) > 0.01f)
            {
                // cast shadow ray
                float3 sampP = (float3)(0.f);
                UniformSampleSphere(GetRandom01(pSeed0, pSeed1),
                                    GetRandom01(pSeed0, pSeed1),
                                    &sampP);
                Ray shadowRay;
                shadowRay.orig = *pHitP;
                shadowRay.dir = light.pos + sampP * light.radius - shadowRay.orig;
                float len = fast_length(shadowRay.dir);
                len -= light.radius;
                shadowRay.dir = normalize(shadowRay.dir);
                if (!IntersectP(pObj, numObjs, &shadowRay, len - 0.01f))
                {
                    // add light contribution
                    *result += light.emission * max(0.0f, dot(*pN, shadowRay.dir));
                }
            }
        }
    }

}

float3 Radiance(const Ray* ray, __constant char* pObj, int numObjs, uint* pSeed0, uint* pSeed1)
{
    Ray currentRay;
    currentRay.orig = ray->orig;
    currentRay.dir = ray->dir;
    int d = 0;
    float3 throughput = (float3)(1.f);
    float3 rad = (float3)(0.f);
    
    while(d < MAX_DEPTH)
    {
        __constant char* pCurr = pObj;
        float t = FLT_MAX;
        bool bHit = false;
        Sphere sHit;
        uint hType;
        for (int i = 0; i < numObjs; ++i)
        {
            uint gtype = *((__constant uint*)pCurr);
            pCurr += sizeof(uint);
            uint gsize = *((__constant uint*)pCurr);
            pCurr += sizeof(uint);

            if (gtype == 1) // SPHERE
            {
                Sphere sp = *((__constant Sphere*)pCurr);
                pCurr += gsize;
                bool tHit = IntersectSphere(&sp, &currentRay, &t);
                if (tHit)
                {
                    sHit = sp;
                    bHit = true;
                    hType = 1;
                }
            }
        }

        if (bHit)
        {
            if (hType == 1) // SPHERE
            {
                // Direct illumination
                throughput *= sHit.color;
                float3 Ld = (float3)(0.f);
                float3 hitP = currentRay.orig + currentRay.dir * t;
                float3 hitN = hitP - sHit.pos;
                hitN = normalize(hitN);
                SampleLights(pObj, numObjs, &hitP, &hitN, &Ld, pSeed0, pSeed1);
                Ld *= throughput;
                rad += Ld + sHit.emission;

                // TODO: Indirect illumination
            }
        }
        else
        {
            break;
        }
        ++d;
    }
    return rad;
}

__kernel void RenderKernel(__global uchar* pOutput, int width, int height,
                           __constant Camera* cam,
                           __constant char* pObjs, int objsSize, int numObjs,
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

        Ray cRay = CastCamRay(px, py, cam, &seed0, &seed1);

        float3 rad = Radiance(&cRay, pObjs, numObjs, &seed0, &seed1);
        float invNs = 1.0f / (currentSample + 1);
        float sNs = (float)currentSample * invNs;
        color[pid * 3]     = sNs * color[pid*3    ] + rad.x * invNs;
        color[pid * 3 + 1] = sNs * color[pid*3 + 1] + rad.y * invNs;
        color[pid * 3 + 2] = sNs * color[pid*3 + 2] + rad.z * invNs;

        pOutput[pid * 3] = color[pid * 3] * 255;
        pOutput[pid * 3 + 1 ] = color[pid * 3 + 1 ] * 255;
        pOutput[pid * 3 + 2 ] = color[pid * 3 + 2 ] * 255;

        pSeeds[pid] = seed0;
        pSeeds[worksize + pid] = seed1;
    }
}
