#pragma once
#include <vector>
#include <string>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>
using Eigen::Vector2f;
using Eigen::Vector2i;
using Eigen::Vector3f;
using namespace std;

/// @brief 書き込み先の画像
class RenderTarget
{
private:
    Vector2i screenSize = Vector2i(100, 100);
    vector<Vector3f> array; // 3次元ベクトルで色を保持

public:
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

    // 出力オペレータのオーバーロード
    friend std::ostream &operator<<(std::ostream &os, const RenderTarget &rt);
};
