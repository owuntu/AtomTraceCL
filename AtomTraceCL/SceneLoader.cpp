#include "DiffuseMaterial.h"
#include "Light.h"
#include "Metal.h"
#include "ObjectList.h"
#include "Plane.h"
#include "RenderObject.h"
#include "SceneLoader.h"
#include "Sphere.h"
#include "TriMesh.h"

namespace AtomTraceCL
{
    // Hard coded cooper data.
    static const Metal s_COOPER(AtomMathCL::Vector3(0.2f, 0.924f, 1.102f), AtomMathCL::Vector3(3.91f, 2.45f, 2.14f), 1000.f);

    // Hard coded gold data.
    static const Metal s_GOLD(AtomMathCL::Vector3(0.143f, 0.375f, 1.44f), AtomMathCL::Vector3(3.983f, 2.386f, 1.603f), 1000.f);

    void LoadCornelBox(ObjectList& oList)
    {
        Plane pl0;
        DiffuseMaterial wall;
        wall.SetColor(AtomMathCL::Vector3(1.0f, 1.0f, 1.0f));

        // back
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(50.f);
            obj.Rotate(90.f, AtomMathCL::Vector3::UNIT_X);
            obj.Translate(0.f, 0.f, -30.f);

            obj.SetMaterial(&wall);
            oList.AddObject(obj);
        }

        // front
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(50.f);
            obj.Rotate(-90.f, AtomMathCL::Vector3::UNIT_X);
            obj.Translate(0.f, 0.f, 70.f);

            obj.SetMaterial(&wall);
            oList.AddObject(obj);
        }
        // left
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(50.f);
            obj.Rotate(-90.f, AtomMathCL::Vector3::UNIT_Z);
            obj.Translate(-15.f, 0.f, 0.f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.25f, 0.75f, 0.25f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }

        // right
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(50.f);
            obj.Rotate(90.f, AtomMathCL::Vector3::UNIT_Z);
            obj.Translate(15.f, .0f, .0f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.75f, 0.25f, 0.25f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }

        // top
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(50.f);
            obj.Rotate(180.f, AtomMathCL::Vector3::UNIT_X);
            obj.Translate(.0f, 0.f, .0f);

            obj.SetMaterial(&wall);
            oList.AddObject(obj);
        }

        // bottom
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(50.f);
            obj.Translate(0.f, -22.f, 0.f);

            obj.SetMaterial(&wall);
            oList.AddObject(obj);
        }
    }

    bool LoadUnitTestObj(ObjectList& oList)
    {
        Sphere s0;

        DiffuseMaterial ball;
        ball.SetColor(AtomMathCL::Vector3(1.f, 0.3f, 0.3f));

        // light
        {
            RenderObject obj;
            obj.SetGeometry(&s0);
            obj.Scale(1.0f);
            obj.Translate(1.0f, -22.f, 15.f);

            Light l0;
            l0.SetEmission(AtomMathCL::Vector3(1.f));
            obj.SetMaterial(&l0);

            oList.AddObject(obj);
        }
#if 0
        // load teapot
        {
            TriMesh tMesh;
            tMesh.LoadObjFromFile("scene\\teapot-low.obj");
            RenderObject obj;
            obj.SetGeometry(&tMesh);
            //obj.Rotate(90.f, AtomMathCL::Vector3::UNIT_X);
            //obj.Translate(0.f, 2.f, 0.f);
            obj.Scale(0.1f);
            obj.SetMaterial(&s_gold);
            oList.AddObject(obj);
        }
#endif
        // load dragon
        {
            TriMesh tMesh;
            tMesh.LoadObjFromFile("scene\\dragon.obj");
            RenderObject obj;
            obj.SetGeometry(&tMesh);
            obj.SetMaterial(&s_GOLD);
            obj.Rotate(180.f, AtomMathCL::Vector3::UNIT_Z);
            obj.Scale(100.f);
            obj.Translate(0.f, 5.5f, 12.f);
            oList.AddObject(obj);
        }

        LoadCornelBox(oList);
        return true;
    }
    bool LoadScene(ObjectList& oList)
    {
        return LoadUnitTestObj(oList);

        Plane pl0;
        Sphere s0;
        DiffuseMaterial wall;
        wall.SetColor(AtomMathCL::Vector3(1.0f, 1.0f, 1.0f));

        DiffuseMaterial ball;
        ball.SetColor(AtomMathCL::Vector3(1.f, 0.3f, 0.3f));

        // light
        {
            RenderObject obj;
            obj.SetGeometry(&s0);
            obj.Scale(1.0f);
            obj.Translate(1.0f, -22.f, 10.f);

            Light l0;
            l0.SetEmission(AtomMathCL::Vector3(1.f));
            obj.SetMaterial(&l0);

            oList.AddObject(obj);
        }
        {
            RenderObject obj;
            obj.SetGeometry(&s0);
            obj.Scale(4.0f);
            obj.Translate(-8.0f, -4.0f, 6.f);
            
            obj.SetMaterial(&s_COOPER);
            oList.AddObject(obj);
        }
        //if (false)
        {
            RenderObject obj;
            obj.SetGeometry(&s0);
            obj.Scale(4.0f);
            obj.Translate(8.0f, -4.0f, 6.f);
            
            obj.SetMaterial(&ball);
            oList.AddObject(obj);
        }

        LoadCornelBox(oList);
        return true;
    }
} // namespace AtomTraceCL
