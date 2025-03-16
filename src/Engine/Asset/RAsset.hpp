#pragma once
#include <filesystem>
using namespace std;
namespace REngine
{
    /// @brief アセット管理のためのインターフェース
    class RAsset
    {
    public:
        ~RAsset() = default;
        virtual bool LoadFromFile(const filesystem::path &filepath) = 0;
    };
}