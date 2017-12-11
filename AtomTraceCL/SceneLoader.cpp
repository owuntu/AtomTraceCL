#include "DiffuseMaterial.h"
#include "Light.h"
#include "Metal.h"
#include "ObjectList.h"
#include "Plane.h"
#include "RenderObject.h"
#include "SceneLoader.h"
#include "Sphere.h"

namespace AtomTraceCL
{
    bool LoadScene(ObjectList& oList)
    {
        Plane pl0;
        Sphere s0;
        // light
        {
            RenderObject obj;
            obj.SetGeometry(&s0);
            obj.Scale(0.5f);
            obj.Translate(0.0f, -6.5f, -2.5f);

            Light l0;
            l0.SetEmission(AtomMathCL::Vector3(1.f));
            obj.SetMaterial(&l0);

            oList.AddObject(obj);
        }
        {
            RenderObject obj;
            obj.SetGeometry(&s0);
            obj.Scale(2.0f);
            obj.Translate(-3.0f, 3.0f, -3.5f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.75f, 0.25f, 0.25f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }
        //if (false)
        {
            RenderObject obj;
            obj.SetGeometry(&s0);
            obj.Scale(2.0f);
            obj.Translate(3.0f, 3.0f, -3.0f);

            Metal m0;
            m0.m_eta = (AtomMathCL::Vector3(0.2f, 0.924f, 1.102f));
            m0.m_kk = AtomMathCL::Vector3(3.91f, 2.45f, 2.14f);
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }

        // back
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(50.f);
            obj.Rotate(90.f, AtomMathCL::Vector3::UNIT_X);
            obj.Translate(0.f, 0.f, -10.5f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.75f, 0.75f, 0.75f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }
#if 1
        // left
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(50.f);
            obj.Rotate(-90.f, AtomMathCL::Vector3::UNIT_Z);
            obj.Translate(-8.f, 0.f, 0.f);

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
            obj.Translate(8.f, .0f, .0f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.25f, 0.25f, 0.75f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }

        // top
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(50.f);
            obj.Rotate(180.f, AtomMathCL::Vector3::UNIT_X);
            obj.Translate(.0f, 5.f, .0f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.75f, 0.75f, 0.75f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }

        // bottom
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(50.f);
            obj.Translate(0.f, -7.f, 0.f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.75f, 0.75f, 0.75f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }
#endif
        return true;
    }
} // namespace AtomTraceCL
