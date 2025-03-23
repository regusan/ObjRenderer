
#pragma once
#include "../../Models/Model.hpp"
#include <memory>
#include <cmath>
namespace REngine
{
    class PrimitiveMeshData
    {
    private:
    public:
        PrimitiveMeshData();
        ~PrimitiveMeshData();

        static std::shared_ptr<Model> GenerateUnitBox();
        static std::shared_ptr<Model> GenerateUnitSphere(int subdivisions = 0);
        static std::shared_ptr<Model> GeneratePlane(float width = 1.0f, float height = 1.0f);
        static std::shared_ptr<Model> GenerateLine(Vector3f start, Vector3f end);
    };
}
