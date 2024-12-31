#include "RenderTarget.hpp"

RenderTarget::RenderTarget()
{
    RenderTarget(100, 100, Vector3f(1, 0, 1));
}
RenderTarget::RenderTarget(string path)
{
    // ファイルを開く
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open PPM file: " + path);
    }

    string format;
    int width = 0, height = 0, maxColorValue = 0;
    string word;
    file >> format;
    file >> width >> height >> maxColorValue;

    if (format != "P3" && format != "P6")
    {
        cerr << path << ": Invalid format, expected P3 or P6, but got " << format << endl;
        exit(1);
    }

    if (width <= 0 || height <= 0 || maxColorValue <= 0)
    {
        cerr << path << ": Invalid header values." << endl;
        cerr << "width;" << width << ",height:" << height << ",maxColorValue:" << maxColorValue << endl;
        exit(1);
    }

    // RenderTargetのサイズと初期化
    screenSize = Vector2i(width, height);
    array.resize(width * height);

    // P6形式の場合は、バイナリデータを読み込む
    if (format == "P6")
    {
        file.read(reinterpret_cast<char *>(array.data()), width * height * 3);
        for (int i = 0; i < width * height; ++i)
        {
            // ピクセルをRGBに変換して、0-1範囲に正規化
            array[i] = Vector3f(
                static_cast<float>(array[i][0]) / maxColorValue,
                static_cast<float>(array[i][1]) / maxColorValue,
                static_cast<float>(array[i][2]) / maxColorValue);
        }
    }
    else if (format == "P3")
    {
        // P3形式の場合は、スペース区切りでRGB値を読み込む
        int r, g, b;
        size_t pixelIndex = 0;
        while (file >> r >> g >> b)
        {
            if (pixelIndex >= static_cast<size_t>(width * height))
            {
                throw std::runtime_error("PPM file contains more data than expected.");
            }
            array[pixelIndex] = Vector3f(
                static_cast<float>(r) / maxColorValue,
                static_cast<float>(g) / maxColorValue,
                static_cast<float>(b) / maxColorValue);
            ++pixelIndex;
        }

        // ピクセル数が足りない場合のチェック
        if (pixelIndex != static_cast<size_t>(width * height))
        {
            cerr << "Expected " << width * height << " pixels, but got " << pixelIndex << endl;
            throw std::runtime_error("PPM file contains insufficient pixel data.");
        }
    }

    file.close(); // ファイルを閉じる
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
const Vector3f RenderTarget::GetMax()
{
    Vector3f retval = Vector3f(0, 0, 0);
    for (Vector3f current : this->array)
    {
        if (current.x() != numeric_limits<float>::max() &&
            current.y() != numeric_limits<float>::max() &&
            current.z() != numeric_limits<float>::max())
            if (current.norm() > retval.norm())
            {
                retval = current;
            }
    }
    return retval;
}
const Vector3f RenderTarget::GetMin()
{
    if (this->array.size() == 0)
        return Vector3f(0, 0, 0);
    Vector3f retval = this->array[0];
    for (Vector3f current : this->array)
    {
        if (current.norm() > retval.norm())
        {
            retval = current;
        }
    }
    return retval;
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
            file << " " << clamp<int>(col.x() * 255, 0, 255)
                 << " " << clamp<int>(col.y() * 255, 0, 255)
                 << " " << clamp<int>(col.z() * 255, 0, 255)
                 << " ";
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

RenderTarget RenderTarget::Abs()
{
    RenderTarget retval = RenderTarget((*this));
    for (size_t i = 0; i < this->array.size(); i++)
    {
        retval.array[i] = retval.array[i].cwiseAbs();
    }
    return retval;
}

RenderTarget RenderTarget::DownSample(const Vector2i size)
{
    RenderTarget retval = RenderTarget(size.x(), size.y());
    for (int y = 0; y < size.y(); y++)
    {
        for (int x = 0; x < size.x(); x++)
        {
            retval.PaintPixel(x, y, this->SampleColor01(static_cast<float>(x) / size.x(), static_cast<float>(y) / size.y()));
        }
    }
    return retval;
}
RenderTarget RenderTarget::UpSample(const Vector2i size)
{
    RenderTarget scaledX = RenderTarget(size.x(), this->screenSize.y());
    RenderTarget scaledXY = RenderTarget(size.x(), size.y());
    for (int y = 0; y < scaledX.screenSize.y(); y++)
    {
        for (int x = 0; x < scaledX.screenSize.x(); x++)
        {
            Vector2f samplePos = Vector2f(static_cast<float>(x) / scaledX.screenSize.x() * this->screenSize.x(), y);
            Vector3f left = this->SampleColor(samplePos.x(), samplePos.y());
            Vector3f right = this->SampleColor(samplePos.x() + 1, samplePos.y());
            float r = samplePos.x() - floor(samplePos.x());
            Vector3f color = r * right + (1 - r) * left;
            scaledX.PaintPixel(x, y, color);
        }
    }
    for (int y = 0; y < scaledXY.screenSize.y(); y++)
    {
        for (int x = 0; x < scaledXY.screenSize.x(); x++)
        {
            Vector2f samplePos = Vector2f(x, static_cast<float>(y) / scaledXY.screenSize.y() * this->screenSize.y());
            Vector3f up = scaledX.SampleColor(samplePos.x(), samplePos.y());
            Vector3f down = scaledX.SampleColor(samplePos.x(), samplePos.y() + 1);
            float r = samplePos.y() - floor(samplePos.y());
            Vector3f color = r * down + (1 - r) * up;
            scaledXY.PaintPixel(x, y, color);
        }
    }
    return scaledXY;
}

RenderTarget RenderTarget::BoxBlur(const int kernelSize, const int kernelScale)
{
    RenderTarget retval = RenderTarget((*this));
    for (int y = 0; y < this->screenSize.y(); y++)
    {
        for (int x = 0; x < this->screenSize.x(); x++)
        {
            Vector3f sum = Vector3f(0, 0, 0);
            for (int lx = -kernelSize / 2; lx < kernelSize / 2; lx++)
            {
                for (int ly = -kernelSize / 2; ly < kernelSize / 2; ly++)
                {
                    sum += this->SampleColor(x + lx * kernelScale, y + ly * kernelScale); // * kernel[abs(lx)] * kernel[abs(ly)];
                }
            }
            retval.PaintPixel(x, y, sum / (kernelSize * kernelSize));
        }
    }
    return retval;
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
RenderTarget operator%(const RenderTarget &rt, const float &val)
{
    RenderTarget retval = RenderTarget(rt);
    for (size_t i = 0; i < rt.array.size(); i++)
    {
        retval.array[i] = Vector3f(fmod(retval.array[i].x(), val),
                                   fmod(retval.array[i].y(), val),
                                   fmod(retval.array[i].z(), val));
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
