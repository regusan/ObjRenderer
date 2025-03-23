#pragma once
namespace REngine
{
    class PhysicMaterial
    {
    public:
        float bounciness = 0.5f;      // 反発係数 (0 = 衝撃吸収, 1 = 完全反射)
        float staticFriction = 0.5f;  // 静止摩擦 (0 = つるつる, 1 = 摩擦大)
        float dynamicFriction = 0.5f; // 動摩擦 (0 = 滑りやすい, 1 = 滑りにくい)
        float density = 1.0f;         // 密度 (質量計算に影響)

        PhysicMaterial() {}
        PhysicMaterial(float bounciness, float staticFriction, float dynamicFriction, float density)
            : bounciness(bounciness), staticFriction(staticFriction), dynamicFriction(dynamicFriction), density(density) {}
        PhysicMaterial Resolve(PhysicMaterial other) const
        {
            PhysicMaterial result;
            result.bounciness = (bounciness + other.bounciness) / 2.0f;
            result.staticFriction = (staticFriction + other.staticFriction) / 2.0f;
            result.dynamicFriction = (dynamicFriction + other.dynamicFriction) / 2.0f;
            result.density = (density + other.density) / 2.0f;
            return result;
        }
    };
    namespace PhysicMaterialExample
    {
        // 反発係数 (bounciness) が低い順にソート
        static const PhysicMaterial Water(0.0f, 0.0f, 0.0f, 1.0f);       // 水: 衝突時の反発なし
        static const PhysicMaterial Ice(0.1f, 0.05f, 0.02f, 0.92f);      // 氷: 非常に低い摩擦
        static const PhysicMaterial Concrete(0.2f, 0.9f, 0.8f, 2.4f);    // コンクリート: 高い摩擦
        static const PhysicMaterial Cloth(0.3f, 0.8f, 0.7f, 0.5f);       // 布: 高い摩擦と軽い
        static const PhysicMaterial Metal(0.3f, 0.7f, 0.5f, 7.8f);       // 金属: 低い反発係数、高い密度
        static const PhysicMaterial Plastic(0.4f, 0.5f, 0.4f, 1.2f);     // プラスチック: 中程度の摩擦と密度
        static const PhysicMaterial Wood(0.5f, 0.6f, 0.4f, 0.9f);        // 木: 中程度の摩擦と密度
        static const PhysicMaterial Leather(0.6f, 0.7f, 0.6f, 0.86f);    // 革: 適度な摩擦
        static const PhysicMaterial Asphalt(0.6f, 0.85f, 0.7f, 2.3f);    // アスファルト: 高摩擦の地面
        static const PhysicMaterial Rubber(0.7f, 1.0f, 0.9f, 1.5f);      // ゴム: 高い摩擦と反発
        static const PhysicMaterial Glass(0.9f, 0.6f, 0.4f, 2.6f);       // ガラス: 高い反発係数、滑りやすい
        static const PhysicMaterial BouncyBall(0.95f, 0.4f, 0.3f, 1.2f); // バウンドボール: ほぼ完全な反発
    }
}