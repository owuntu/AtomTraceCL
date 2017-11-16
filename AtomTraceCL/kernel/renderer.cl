typedef struct
{
    float3 origin;
    float3 direction;
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
    //float3 lookat;
    //float3 right;
    //float3 up;
    float3 pos;
    float3 upLeftPix;
    float3 dx; // camera right direction, per-pixel width
    float3 dy; // camera up direction, per-pixel height
}Camera;

// fov is the vertial fov angle
Ray castCamRay(int px, int py, __const Camera* cam)
{
#if 0
    // For simplicity, the image plane is assume at (0,0,0) with normal (0, 0, -1), and camera at (0,0,1)
    float hv = tan(fov * 0.5f * M_PI / 180.0f);
    float asp = (float)width / (float)height;
    float hu = hv * asp;

    float tlx = -hu; // top left pixel x
    float tly = hv;

    float fx = (float)px / (float)(width-1);
    float fy = (float)py / (float)(height-1);

    float3 tar = (float3)(tlx + hu * 2.f * fx,
                          tly - hv * 2.f * fy,
                          0.f);
    float3 orig = (float3)(0.0f, 0.0f, 1.0f);
#endif
    float fx = (float)px;
    float fy = (float)py;
    float3 tar = cam->upLeftPix 
                 + cam->dx * fx
                 - cam->dy * fy;

    float3 orig = cam->pos;

    Ray ray;
    ray.direction = tar - orig;
    ray.direction = normalize(ray.direction);
    ray.origin = orig;
    return ray;
}

bool intersectSphere(const Sphere* obj, const Ray* ray)
{
    bool res = false;
    float3 po = obj->pos - ray->origin;
    float b = dot(po, ray->direction);
    float det = b*b - dot(po, po) + obj->radius * obj->radius;
    if (det < 0.0f)
    {
        return false;
    }
    return true;
}

__kernel void render(__global uchar* pOutput, int width, int height, __global const Camera* cam)
{
    int pid = get_global_id(0);
    
    if (pid < width * height)
    {
        pOutput[pid * 3] = 0;
        pOutput[pid * 3 + 1] = 0;
        pOutput[pid * 3 + 2] = 0;

        int px = pid % width;
        int py = pid / width;

        Camera lcam;
#if 1
        lcam.pos = cam->pos;
        lcam.upLeftPix = cam->upLeftPix;
        lcam.dx = cam->dx;
        lcam.dy = cam->dy;
#else
        lcam.pos = (float3)(0.f, 0.f, 1.f);
        lcam.upLeftPix = (float3)(-1.0f, 0.75f, 0.0f);
        lcam.dx = (float3)(0.0025f, 0.f, 0.f);
        lcam.dy = (float3)(0.f, 0.0025f, 0.f);
#endif

        Ray cRay = castCamRay(px, py, &lcam);
        Sphere s0;
        s0.pos = (float3)(0.0f, 0.0f, -2.0f);
        s0.radius = 0.1f;

        bool bHit = intersectSphere(&s0, &cRay);
        uchar val = bHit * 255;
        pOutput[pid * 3] = val;
        pOutput[pid * 3 + 1] = val;
        pOutput[pid * 3 + 2] = val;
    }
}