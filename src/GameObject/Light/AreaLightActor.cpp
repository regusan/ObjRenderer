#include "AreaLightActor.hpp"
namespace REngine::Light
{
    Vector3f AreaLightActor::lightSDF(const Vector3f &positionWS, const Vector3f &normalWS)
    {
        // 原点中心でX-Z面に投影するボックスライトで考えるため、入力posとnormをオブジェクトスペースへ変換
        Matrix4f invModelMat = this->getWorldMat().inverse();
        Vector4f positionWS4 = Vector4f(positionWS.x(), positionWS.y(), positionWS.z(), 1);
        Vector3f positionOS = (invModelMat * positionWS4).head<3>();
        Vector3f normalOS = (invModelMat.block<3, 3>(0, 0) * normalWS).normalized(); // 3x3で位置情報を削除

        float offset = fminf(-positionOS.y() * tan(this->barnDoorAngle / 180.0f * M_PI), this->length); // アングルに基づき拡張
        float falloffRate = saturate(1 - fabs(positionOS.x() / (this->size.x() / 2 + offset))) *
                            saturate(1 - fabs(positionOS.z() / (this->size.y() / 2 + offset)));
        float distRate = powf(saturate(1.0f - -positionOS.y() / this->length) * (positionOS.y() <= 0), 2);
        float normRate = saturate(normalOS.dot(Vector3f(0, 1, 0)) * 0.5 + 0.5); // half-lambert
        return color * distRate * normRate * pow(falloffRate, this->falloff);
    }
    float AreaLightActor::GetBoundingSphereRadius()
    {
        return this->length;
    }
    AreaLightActor::AreaLightActor(json args) : LightBaseActor(args)
    {
        this->barnDoorAngle = args["barnDoorAngle"];
        this->length = args["length"];
        this->size.x() = args["size"][0];
        this->size.y() = args["size"][1];
        this->falloff = args["falloff"];
    }
    AreaLightActor::~AreaLightActor() {}
}