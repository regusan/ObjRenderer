#pragma once
#include <vector>
#include <string>
#include "../header/EigenHeader.hpp"
using namespace std;

/// @brief 書き込み先の画像
class RenderTarget
{
private:
    Vector2i screenSize = Vector2i(100, 100);
    vector<Vector3f> array; // 3次元ベクトルで色を保持

public:
    RenderTarget();
    RenderTarget(const int &width, const int &height);
    RenderTarget(const int &width, const int &height, const Vector3f &color);
    ~RenderTarget();

    const Vector2i getScreenSize();
    void Fill(const Vector3f &color);

    // ピクセルに色を塗る関数
    void PaintPixel(const int &x, const int &y, const Vector3f &color);

    // 直線を描画する関数（例示）
    void DrawLine(const Vector2f &start, const Vector2f &end, const Vector3f &color);

    void DrawPolygonWireframe(const vector<Vector2f> &points, const Vector3f &color);

    // ファイルに出力する関数
    void writeAsPPM(const string &filepath);

    float FindMaxEuclideanDistance();
    void mul();

    // 出力オペレータのオーバーロード
    friend RenderTarget operator*(const RenderTarget &rt, const float &mul);
    friend RenderTarget operator%(const RenderTarget &rt, const float &mul);
    friend std::ostream &operator<<(std::ostream &os, const RenderTarget &rt);
};
