#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
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
    /// @brief 指定パスのPPMを取り込む
    /// @param path
    RenderTarget(string path);
    RenderTarget(const int &width, const int &height);
    RenderTarget(const int &width, const int &height, const Vector3f &color);
    ~RenderTarget();

    const Vector2i getScreenSize();
    void Fill(const Vector3f &color);

    /// @brief 指定したピクセルに色を塗る。範囲外を指定すると何もしない
    /// @param x
    /// @param y
    /// @param color
    inline void PaintPixel(const int &x, const int &y, const Vector3f &color)
    {
        if (x >= 0 && x < screenSize.x() && y >= 0 && y < screenSize.y())
        {
            array[y * screenSize.x() + x] = color; // ピクセルに色を設定
        }
    }

    /// @brief 線分を描画
    /// @param start
    /// @param end
    /// @param color
    void DrawLine(const Vector2f &start, const Vector2f &end, const Vector3f &color);

    void DrawPolygonWireframe(const vector<Vector2f> &points, const Vector3f &color);

    /// @brief 指定パスにPPMファイルとして出力
    /// @param filepath
    void writeAsPPM(const string &filepath);

    float FindMaxEuclideanDistance();

    const Vector3f GetMax();
    const Vector3f GetMin();

    /// @brief 色をサンプリング。範囲外ならエラー色を返す
    /// @param x
    /// @param y
    /// @return 色
    inline const Vector3f SampleColor(const int x, const int y)
    {
        size_t index = y * screenSize.x() + x;
        if (index < this->array.size())
        {
            return array[index]; // ピクセルに色を設定
        }
        return Vector3f(1, 0, 1);
    }
    inline const Vector3f SampleColor01(const float x, const float y)
    {
        int x01 = x * screenSize.x();
        int y01 = y * screenSize.y();
        size_t index = y01 * screenSize.x() + x01;
        if (index < this->array.size())
        {
            return array[index]; // ピクセルに色を設定
        }
        return Vector3f(1, 0, 1);
    }
    /// @brief RenderTargetの各ピクセルにabs()を適用
    /// @return
    RenderTarget Abs();
    // 出力オペレータのオーバーロード
    friend RenderTarget operator*(const RenderTarget &rt, const float &mul);
    friend RenderTarget operator%(const RenderTarget &rt, const float &mul);
    friend std::ostream &operator<<(std::ostream &os, const RenderTarget &rt);
};
