struct Ray
{
    float3 origin;
    float3 direction;
};

struct Sphere
{
    float radius;
    float3 pos;
    float3 emission;
    float3 color;
};

// fov is the vertial fov angle
struct Ray castCamRay(int px, int py, int width, int height, float fov)
{
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

    struct Ray ray;
    ray.direction = tar - orig;
    ray.direction = normalize(ray.direction);
    ray.origin = orig;
    return ray;
}

bool intersectSphere(const struct Sphere* obj, const struct Ray* ray)
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

__kernel void render(__global uchar* pOutput, int width, int height)
{
    int pid = get_global_id(0);
    
    if (pid < width * height)
    {
        pOutput[pid * 3] = 0;
        pOutput[pid * 3 + 1] = 0;
        pOutput[pid * 3 + 2] = 0;

        int px = pid % width;
        int py = pid / width;

        struct Ray cRay = castCamRay(px, py, width, height, 40.0f);
        struct Sphere s0;
        s0.pos = (float3)(0.0f, 0.0f, -2.0f);
        s0.radius = 0.1f;

        bool bHit = intersectSphere(&s0, &cRay);
        uchar val = bHit * 255;
        pOutput[pid * 3] = val;
        pOutput[pid * 3 + 1] = val;
        pOutput[pid * 3 + 2] = val;
    }
}