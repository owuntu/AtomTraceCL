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

        Metal metal;
        // Hard coded cooper data.
        metal.m_eta = (AtomMathCL::Vector3(0.2f, 0.924f, 1.102f));
        metal.m_kk = AtomMathCL::Vector3(3.91f, 2.45f, 2.14f);

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
            obj.SetMaterial(&ball);
            oList.AddObject(obj);
        }
#endif
        // load dragon
        {
            TriMesh tMesh;
            tMesh.LoadObjFromFile("scene\\dragon.obj");
            RenderObject obj;
            obj.SetGeometry(&tMesh);
            obj.SetMaterial(&metal);
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
        
        Metal metal;
        // Hard coded cooper data.
        metal.m_eta = (AtomMathCL::Vector3(0.2f, 0.924f, 1.102f));
        metal.m_kk = AtomMathCL::Vector3(3.91f, 2.45f, 2.14f);

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
            
            obj.SetMaterial(&metal);
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
