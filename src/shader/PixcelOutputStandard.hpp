#pragma once

#include "../header/EigenHeader.hpp"

/// @brief ピクセルシェーダー用の出力データクラス
class PixcelOutputStandard
{
private:
    /* data */
public:
    PixcelOutputStandard(/* args */) {}
    ~PixcelOutputStandard() {}
    Vector3f color;
    Vector3f diffuse;
    Vector3f normal;
    Vector3f specular;
    Vector3f emission;
    Vector3f ORM;
};
