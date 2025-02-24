#include "TorusLightActor.hpp"

Vector3f TorusLightActor::lightSDF(const Vector3f &positionWS, const Vector3f &normalWS)
{
    // 原点中心でX-Z面に円状になるトーラスで考えるため、入力posとnormをオブジェクトスペースへ変換
    Matrix4f invModelMat = this->getMat().inverse();
    Vector4f positionWS4 = Vector4f(positionWS.x(), positionWS.y(), positionWS.z(), 1);
    Vector3f positionOS = (invModelMat * positionWS4).head<3>();
    Vector3f normalOS = (invModelMat.block<3, 3>(0, 0) * normalWS).normalized(); // 3x3で位置情報を削除

    Vector3f circlePos = Vector3f(positionOS.x(), 0, positionOS.z()).normalized() * this->majorRadius;
    Vector3f dir2CirclePos = (circlePos - positionOS).normalized();

    // 向きによる明るさ比率(HalfLambert)
    float dirRate = saturate(dir2CirclePos.dot(normalOS) * 0.5 + 0.5);

    // 距離による明るさ比率
    float distRate = saturate(1 - (circlePos - positionOS).norm() / this->minorRadius);
    return color * distRate * dirRate;
}

float TorusLightActor::GetBoundingSphereRadius()
{
    return this->majorRadius + this->minorRadius;
}

TorusLightActor::TorusLightActor(json args) : LightBaseActor(args)
{
    this->majorRadius = args["majorRadius"];
    this->minorRadius = args["minorRadius"];
}
TorusLightActor::~TorusLightActor() {}