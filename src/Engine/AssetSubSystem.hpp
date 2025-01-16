#pragma once

#include "../lib/ReguLib/RAssetManager.hpp"
#include "../Rendering/RenderTarget.hpp"

// #include "../Models/Model.hpp"
class Model; // 循環参照の回避

/// @brief  各アセットマネージャーへの参照を持つ
class AssetSubSystem
{
public:
    static AssetSubSystem &getInstance()
    {
        static AssetSubSystem instance;
        return instance;
    }
    RAssetManager<RenderTarget> textureManager;
    RAssetManager<Model> meshManager;

protected:
    // コンストラクタを private にして外部からのインスタンス化を禁止
    AssetSubSystem() { std::cout << "AssetSubSystem initialized!" << std::endl; }

private:
    // コピーコンストラクタと代入演算子を削除してコピーを禁止
    AssetSubSystem(const AssetSubSystem &) = delete;
    AssetSubSystem &operator=(const AssetSubSystem &) = delete;
};