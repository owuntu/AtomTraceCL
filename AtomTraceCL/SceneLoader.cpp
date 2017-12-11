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
            obj.Scale(0.5f, 0.05f, 0.01f);
            obj.Translate(0.0f, -0.65f, -1.5f);

            Light l0;
            l0.SetEmission(AtomMathCL::Vector3(1.f));
            obj.SetMaterial(&l0);

            oList.AddObject(obj);
        }
        {
            RenderObject obj;
            obj.SetGeometry(&s0);
            obj.Scale(0.3f);
            obj.Translate(-0.3f, 0.22f, -2.0f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.75f, 0.25f, 0.25f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }

        // back
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(5.f);
            obj.Rotate(90.f, AtomMathCL::Vector3::UNIT_X);
            obj.Translate(0.f, 0.f, -2.5f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.75f, 0.75f, 0.75f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }

        // left
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(5.f);
            obj.Rotate(-90.f, AtomMathCL::Vector3::UNIT_Z);
            obj.Translate(-.8f, 0.f, 0.f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.25f, 0.75f, 0.25f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }

        // right
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(5.f);
            obj.Rotate(90.f, AtomMathCL::Vector3::UNIT_Z);
            obj.Translate(.8f, .0f, .0f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.25f, 0.25f, 0.75f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }

        // top
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(5.f);
            obj.Rotate(180.f, AtomMathCL::Vector3::UNIT_X);
            obj.Translate(.0f, .5f, .0f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.55f, 0.55f, 0.55f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }

        // bottom
        {
            RenderObject obj;
            obj.SetGeometry(&pl0);
            obj.Scale(5.f);
            obj.Translate(0.f, -0.7f, 0.f);

            DiffuseMaterial m0;
            m0.SetColor(AtomMathCL::Vector3(0.75f, 0.75f, 0.75f));
            obj.SetMaterial(&m0);
            oList.AddObject(obj);
        }

        return true;
    }
} // namespace AtomTraceCL
