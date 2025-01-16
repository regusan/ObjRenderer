#pragma once
#include <filesystem>
using namespace std;

/// @brief アセット管理のためのインターフェース
class RAsset
{
public:
    ~RAsset() = default;
    virtual bool LoadFromFile(const filesystem::path &filepath) = 0;
};