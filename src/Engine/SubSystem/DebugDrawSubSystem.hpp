#pragma once
#include <iostream>
#include <list>
#include "../Data/PrimitiveMeshData.hpp"
#include "../../GameObject/Mesh.hpp"
namespace REngine
{
    using namespace std;
    /// @brief  各アセットマネージャーへの参照を持つ
    class DebugDrawSubSystem
    {
    public:
        static DebugDrawSubSystem &getInstance()
        {
            static DebugDrawSubSystem instance;
            return instance;
        }

        void InitMesh(shared_ptr<MeshActor> mesh, shared_ptr<Model> model, Vector3f position, Vector3f rotation, Vector3f size, Vector3f color)
        {
            mesh->meshModel = model;
            mesh->meshModel->defaultMaterial.diffuse = color;
            mesh->SetLocalPosition(position);
            mesh->SetLocalRotation(rotation);
            mesh->SetLocalScale(size);
            wireFrameDrawCall.push_back(mesh);
        }
        void DrawDebugBox(Vector3f position, Vector3f rotation, Vector3f size, Vector3f color = Vector3f::Ones())
        {
            static const shared_ptr<Model> meshData = PrimitiveMeshData::GenerateUnitBox();
            shared_ptr meshActor = make_shared<MeshActor>();
            this->InitMesh(meshActor, meshData, position, rotation, size, color);
        }
        void DrawDebugSphere(float radius, Vector3f position, Vector3f rotation, Vector3f size = Vector3f::Ones(), Vector3f color = Vector3f::Ones())
        {
            const shared_ptr<Model> meshData = PrimitiveMeshData::GenerateUnitSphere();
            shared_ptr meshActor = make_shared<MeshActor>();
            this->InitMesh(meshActor, meshData, position, rotation, size, color);
        }
        void DrawDebugLine(Vector3f start, Vector3f end, Vector3f color = Vector3f::Ones())
        {
            const shared_ptr<Model> meshData = PrimitiveMeshData::GenerateLine(start, end);
            shared_ptr meshActor = make_shared<MeshActor>();
            this->InitMesh(meshActor, meshData, Vector3f::Zero(), Vector3f::Zero(), Vector3f::Ones(), color);
        }
        void DrawDebugPlane(Vector3f position, Vector3f rotation, Vector3f size, Vector3f color = Vector3f::Ones())
        {
            static const shared_ptr<Model> meshData = PrimitiveMeshData::GeneratePlane();
            shared_ptr meshActor = make_shared<MeshActor>();
            this->InitMesh(meshActor, meshData, position, rotation, size, color);
        }

        list<shared_ptr<MeshActor>> wireFrameDrawCall;

    protected:
        // コンストラクタを private にして外部からのインスタンス化を禁止
        DebugDrawSubSystem() {}

    private:
        // コピーコンストラクタと代入演算子を削除してコピーを禁止
        DebugDrawSubSystem(const DebugDrawSubSystem &) = delete;
        DebugDrawSubSystem &operator=(const DebugDrawSubSystem &) = delete;
        friend std::ostream &operator<<(std::ostream &os, const DebugDrawSubSystem &as)
        {
            os << "DebugDrawSubSystem(Que=" << as.wireFrameDrawCall.size() << ")" << endl;

            return os;
        }
    };
}