#include "RenderTarget.hpp"
#include <fstream>
#include <iostream>

RenderTarget::RenderTarget()
{
    RenderTarget(100, 100, Vector3f(255, 0, 255));
}
RenderTarget::RenderTarget(const int &width, const int &height)
    : screenSize(width, height), array(width * height) {}

RenderTarget::RenderTarget(const int &width, const int &height, const Vector3f &color)
    : RenderTarget(width, height)
{
    this->Fill(color);
}
RenderTarget::~RenderTarget() {}

const Vector2i RenderTarget::getScreenSize()
{
    return this->screenSize;
}
void RenderTarget::Fill(const Vector3f &color)
{
    this->array = vector(this->screenSize.x() * this->screenSize.y(), color);
}
void RenderTarget::PaintPixel(const int &x, const int &y, const Vector3f &color)
{
    if (x >= 0 && x < screenSize.x() && y >= 0 && y < screenSize.y())
    {
        array[y * screenSize.x() + x] = color; // ピクセルに色を設定
    }
}

void RenderTarget::DrawLine(const Vector2f &start, const Vector2f &end, const Vector3f &color)
{
    Vector2i startI = start.cast<int>();
    Vector2i endI = end.cast<int>();
    Vector2i d = (endI - startI).cwiseAbs();
    Vector2i s = Vector2i((startI.x() < endI.x()) ? 1 : -1, (startI.y() < endI.y()) ? 1 : -1);
    int err = d.x() - d.y();

    while (true)
    {
        this->PaintPixel(startI.x(), startI.y(), color); // 現在の位置にピクセルを描画

        if (startI.x() == endI.x() && startI.y() == endI.y())
            break;

        int e2 = err * 2;

        if (e2 > -d.y())
        {
            err -= d.y();
            startI.x() += s.x();
        }

        if (e2 < d.x())
        {
            err += d.x();
            startI.y() += s.y();
        }
    }
}

void RenderTarget::DrawPolygonWireframe(const vector<Vector2f> &points, const Vector3f &color)
{
    if (points.size() >= 2)
    {
        this->DrawLine(points[0], points[points.size() - 1], color); // 先頭と末尾の接続
        for (size_t i = 0; i < points.size() - 1; i++)
        {
            this->DrawLine(points[i], points[i + 1], color);
        }
    }
}

void RenderTarget::writeAsPPM(const string &filepath)
{
    std::ofstream file(filepath);
    if (!file)
    {
        std::cerr << "Error: Unable to open file for writing!" << std::endl;
        return;
    }

    file << "P3\n"
         << this->screenSize.x() << " " << this->screenSize.y() << " 255\n";
    for (int y = 0; y < this->screenSize.y(); y++)
    {
        for (int x = 0; x < this->screenSize.x(); x++)
        {
            Vector3f col = this->array[y * this->screenSize.y() + x];
            file << " " << clamp((int)col.x(), 0, 255) << " " << clamp((int)col.y(), 0, 255) << " " << clamp((int)col.z(), 0, 255) << " ";
        }
        file << "\n";
    }
    file.close();
}

float RenderTarget::FindMaxEuclideanDistance()
{
    float maxDistance = 0.0f; // 最大距離を格納する変数

    // 全ペアを探索して最大距離を探す
    for (size_t i = 0; i < this->array.size(); ++i)
    {
        for (size_t j = i + 1; j < this->array.size(); ++j)
        { // ペアを1回ずつだけ比較
            float distance = (this->array[i] - this->array[j]).norm();
            maxDistance = max(maxDistance, distance); // 最大値を更新
        }
    }
    return maxDistance;
}
RenderTarget operator*(const RenderTarget &rt, const float &mul)
{
    RenderTarget retval = RenderTarget(rt);
    for (size_t i = 0; i < rt.array.size(); i++)
    {
        retval.array[i] = retval.array[i] * mul;
    }
    return retval;
}
std::ostream &operator<<(std::ostream &os, const RenderTarget &rt)
{
    for (int y = 0; y < rt.screenSize.y(); ++y)
    {
        for (int x = 0; x < rt.screenSize.x(); ++x)
        {
            Vector3f col = rt.array[y * rt.screenSize.x() + x];
            os << "(" << col.x() << "," << col.y() << "," << col.z() << ") ";
        }
        os << "\n";
    }
    return os;
}
