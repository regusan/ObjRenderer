#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include "RAsset.hpp"

/// @brief アセット管理クラス
/// @tparam AssetType
template <typename AssetType>
class RAssetManager
{
public:
    /// @brief アセットのロード（すでにロード済みならキャッシュを使用）
    /// @param assetPath
    /// @return
    std::shared_ptr<AssetType> LoadAsset(const std::string &assetPath)
    {
        // すでにロード済みならキャッシュを返す
        auto it = assetCache.find(assetPath);
        if (it != assetCache.end())
        {
            if (it->second)
                return it->second;
            else
            {
                cerr << "Asset found in cache but is nullptr: " << assetPath << endl;
            }
        }

        // 新しいアセットをロード
        shared_ptr<AssetType> asset = std::make_shared<AssetType>();
        RAsset *rasset = dynamic_cast<RAsset *>(asset.get());

        if (!rasset->LoadFromFile(assetPath))
        {
            throw runtime_error("Failed to load asset: " + assetPath);
        }

        // キャッシュに登録して返す
        assetCache[assetPath] = asset;
        return asset;
    }

    /// @brief  指定アセットをアンロード
    void unloadAsset(const std::string &assetPath)
    {
        assetCache.erase(assetPath);
    }

    /// @brief  キャッシュをすべてクリア
    void clearCache()
    {
        assetCache.clear();
    }

    friend std::ostream &operator<<(std::ostream &os, const RAssetManager &rt)
    {
        os << "Asset cache size: " << rt.assetCache.size() << endl;
        for (const auto &pair : rt.assetCache)
        {
            os << "Path:" << pair.first
               << "\t\tCount:" << pair.second.use_count()
               << "\tValid:" << (pair.second ? "Yes" : "No")
               << endl;
        }
        return os;
    }

private:
    unordered_map<string, shared_ptr<AssetType>> assetCache;
};
