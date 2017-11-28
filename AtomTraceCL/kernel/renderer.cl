#define MAX_DEPTH 1

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

// fov is the vertial fov angle
Ray CastCamRay(int px, int py, __constant Camera* cam)
{
    float fx = (float)px;
    float fy = (float)py;
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

bool IntersectP(__constant char* pObj, int numObjs, const Ray* ray, const float maxt)
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
            bHit |= IntersectSphere(&sp, ray, &t);
        }
    }
    return bHit;
}

void SampleLights(__constant char* pObj, int numObjs,
                  const float3* pHitP, const float3* pN, float3* result)
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
            Sphere sp = *(__constant Sphere*)pCurr;
            if (fast_length(sp.emission) > 0.01f)
            {
                // assuming point light for now
                // cast shadow ray
                Ray shadowRay;
                shadowRay.orig = *pHitP;
                shadowRay.dir = sp.pos - shadowRay.orig;
                float len = fast_length(shadowRay.dir);
                len -= sp.radius;
                shadowRay.dir = normalize(shadowRay.dir);
                if (!IntersectP(pObj, numObjs, &shadowRay, len - 0.01f))
                {
                    // add light contribution
                    *result += sp.emission * max(0.0f, dot(*pN, shadowRay.dir));
                }
            }
        }
    }

}

float3 Radiance(const Ray* ray, __constant char* pObj, int numObjs)
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
                SampleLights(pObj, numObjs, &hitP, &hitN, &Ld);
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
                           __constant char* pObjs, int objsSize, int numObjs)
{
    int pid = get_global_id(0);
    
    if (pid < width * height)
    {
        int px = pid % width;
        int py = pid / width;

        Ray cRay = CastCamRay(px, py, cam);

        float3 rad = Radiance(&cRay, pObjs, numObjs);
        
        pOutput[pid * 3] = rad.x * 255;
        pOutput[pid * 3 + 1] = rad.y * 255;
        pOutput[pid * 3 + 2] = rad.z * 255;
    }
}
