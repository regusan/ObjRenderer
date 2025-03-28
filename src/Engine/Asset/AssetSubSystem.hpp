#pragma once

#include "RAssetManager.hpp"

class Model;        // 循環参照の回避
class RenderTarget; // 循環参照の回避

namespace REngine
{
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
        AssetSubSystem();

    private:
        // コピーコンストラクタと代入演算子を削除してコピーを禁止
        AssetSubSystem(const AssetSubSystem &) = delete;
        AssetSubSystem &operator=(const AssetSubSystem &) = delete;
        friend std::ostream &operator<<(std::ostream &os, const AssetSubSystem &as)
        {
            os << "AssetSubSystem" << endl;
            os << "TextureAssets" << endl
               << as.textureManager << endl;
            os << "MeshAssets" << endl
               << as.meshManager << endl;
            return os;
        }
    };
}