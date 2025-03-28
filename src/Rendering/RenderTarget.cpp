#include "RenderTarget.hpp"

RenderTarget::RenderTarget()
{
}
RenderTarget::RenderTarget(filesystem::path filepath)
{
    this->LoadFromFile(filepath);
}

bool RenderTarget::LoadFromFile(const filesystem::path &filepath)
{
    if (!std::filesystem::exists(filepath))
    {
        cerr << "RenderTarget::テクスチャ\"" << filepath.root_path().string() << "\"の読み込みに失敗しました。" << endl;
        return false;
    }
    if (filepath.extension() == ".ppm")
    {
        this->ReadFromPPM(filepath);
        return true;
    }
    this->ReadWithStb(filepath);
    return true;
}

void RenderTarget::ReadWithStb(filesystem::path filepath)
{
    int channel;
    float *pixels = stbi_loadf(filepath.string().c_str(), &this->screenSize.x(), &this->screenSize.y(), &channel, 0);
    this->array.resize(this->screenSize.x() * this->screenSize.y());
    for (int i = 0; i < this->screenSize.x() * this->screenSize.y(); i++)
    {
        int pixIndex = channel * i;
        if (channel >= 3)
            this->array[i] = Vector3f(pixels[pixIndex], pixels[pixIndex + 1], pixels[pixIndex + 2]);
        if (channel == 2)
            this->array[i] = Vector3f(pixels[pixIndex], pixels[pixIndex + 1], 1);
        if (channel == 1)
            this->array[i] = Vector3f(pixels[pixIndex], pixels[pixIndex], pixels[pixIndex]);
    }
    stbi_image_free(pixels);
}
void RenderTarget::ReadFromPPM(filesystem::path filepath)
{

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open PPM file: " + filepath.root_path().string());
    }

    string format;
    int width = 0, height = 0, maxColorValue = 0;
    string word;
    file >> format;
    file >> width >> height >> maxColorValue;

    if (format != "P3" && format != "P6")
    {
        cerr << filepath.root_path().string() << ": Invalid format, expected P3 or P6, but got " << format << endl;
        exit(1);
    }

    if (width <= 0 || height <= 0 || maxColorValue <= 0)
    {
        cerr << filepath.root_path().string() << ": Invalid header values." << endl;
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
    : screenSize(width, height), array(width * height, color), resetColor(color) {}

RenderTarget::~RenderTarget() {}

const Vector2i RenderTarget::getScreenSize() const
{
    return this->screenSize;
}
void RenderTarget::Fill(const Vector3f &color)
{
    fill(this->array.begin(), this->array.end(), color);
}

void RenderTarget::DrawLine(const Vector2f &start, const Vector2f &end, const Vector3f &color)
{
    constexpr int skip = 1;
    Vector2f delta = end - start;
    float steps = fmax(fabs(delta.x()), fabs(delta.y()));
    Vector2f inc = delta / steps * skip;
    Vector2f current = start;
    for (int i = 0; i < steps; i += skip)
    {
        this->PaintPixel(current.x(), current.y(), color);
        current += inc;
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
Vector3f RenderTarget::GetMax() const
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
Vector3f RenderTarget::GetMin() const
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
void RenderTarget::writeAsPPM(const string &filepath) const
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
void RenderTarget::writeAsPNG(const filesystem::path filepath) const
{
    int channel = 3;
    vector<unsigned char> pixels(channel * this->screenSize.x() * this->screenSize.y());
    for (int i = 0; i < this->screenSize.x() * this->screenSize.y(); i++)
    {
        int pixIndex = channel * i;
        pixels[pixIndex + 0] = static_cast<unsigned char>(clamp<int>(this->array[i].x() * 255, 0, 255));
        pixels[pixIndex + 1] = static_cast<unsigned char>(clamp<int>(this->array[i].y() * 255, 0, 255));
        pixels[pixIndex + 2] = static_cast<unsigned char>(clamp<int>(this->array[i].z() * 255, 0, 255));
    }
    stbi_write_png(filepath.string().c_str(), this->screenSize.x(), this->screenSize.y(), 3, pixels.data(), this->screenSize.x() * 3);
}
float RenderTarget::FindMaxEuclideanDistance() const
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

RenderTarget RenderTarget::Abs() const
{
    RenderTarget retval = RenderTarget((*this));
    for (size_t i = 0; i < this->array.size(); i++)
    {
        retval.array[i] = retval.array[i].cwiseAbs();
    }
    return retval;
}

RenderTarget RenderTarget::DownSample(const Vector2i size) const
{
    RenderTarget retval = RenderTarget(size.x(), size.y());
    int ratioX = this->screenSize.x() / size.x();
    int ratioY = this->screenSize.y() / size.y();

    for (int y = 0; y < size.y(); y++)
    {
        for (int x = 0; x < size.x(); x++)
        {
            Vector3f sum = Vector3f(0, 0, 0);
            for (int dy = 0; dy < ratioY; dy++)
            {
                for (int dx = 0; dx < ratioX; dx++)
                {
                    sum += this->SampleColor(x * ratioX + dx, y * ratioY + dy);
                }
            }
            retval.PaintPixel(x, y, sum / (ratioX * ratioY));
        }
    }
    return retval;
}

RenderTarget RenderTarget::UpSample(const Vector2i size) const
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

RenderTarget RenderTarget::BoxBlur(const int kernelSize, function<bool(Vector3f)> shouldSample, const bool isLoopingX, const bool isLoopingY, const int kernelScale) const
{
    RenderTarget retval = RenderTarget((*this));
    RenderTarget intermediate = RenderTarget((*this));

    int actualKernelSize = clamp<int>(kernelSize / 2 * 2 + 1, 3, min(this->screenSize.x() / 2, this->screenSize.y() / 2));

    // 横方向のぼかし
    for (int y = 0; y < this->screenSize.y(); y++)
    {
        for (int x = 0; x < this->screenSize.x(); x++)
        {
            if (!shouldSample(this->SampleColor(x, y)))
                continue;
            Vector3f sum = Vector3f(0, 0, 0);
            int sampledCount = 1;
            for (int lx = -actualKernelSize / 2; lx <= actualKernelSize / 2; lx++)
            {
                int sampleX;
                if (isLoopingX)
                    sampleX = (x + lx) % this->screenSize.x();
                else
                    sampleX = clamp<int>(x + lx, 0, this->screenSize.x() - 1);
                Vector3f sampled = this->SampleColor(sampleX, y);
                if (shouldSample(sampled))
                {
                    sum += sampled;
                    sampledCount++;
                }
            }
            sum /= sampledCount;
            intermediate.PaintPixel(x, y, sum);
        }
    }

    // 縦方向のぼかし
    for (int y = 0; y < this->screenSize.y(); y++)
    {
        for (int x = 0; x < this->screenSize.x(); x++)
        {
            if (!shouldSample(this->SampleColor(x, y)))
                continue;
            Vector3f sum = Vector3f(0, 0, 0);
            int sampledCount = 0;
            for (int ly = -actualKernelSize / 2; ly <= actualKernelSize / 2; ly++)
            {
                int sampleY;
                if (isLoopingY)
                    sampleY = (y + ly) % this->screenSize.y();
                else
                    sampleY = clamp<int>(y + ly, 0, this->screenSize.y() - 1);
                Vector3f sampled = this->SampleColor(x, sampleY);
                if (shouldSample(sampled))
                {
                    sum += sampled;
                    sampledCount++;
                }
            }
            sum /= sampledCount;
            retval.PaintPixel(x, y, sum);
        }
    }

    return retval;
}
RenderTarget RenderTarget::GausiannBlur(const int kernelSize, const bool isLoopingX, const bool isLoopingY, const int kernelScale) const
{
    RenderTarget retval = RenderTarget((*this));
    RenderTarget intermediate = RenderTarget((*this));

    int actualKernelSize = clamp<int>(kernelSize / 2 * 2 + 1, 3, min(this->screenSize.x() / 2, this->screenSize.y() / 2));
    vector<float> kernel(actualKernelSize, 0.0f);

    // ガウスカーネルの生成
    float sigma = static_cast<float>(kernelSize) * 0.5f;
    float gaussSum = 0.0f;
    for (int i = 0; i < actualKernelSize; i++)
    {
        int x = i - actualKernelSize / 2;
        kernel[i] = exp(-0.5f * (x * x) / (sigma * sigma));
        gaussSum += kernel[i];
    }
    // 正規化
    for (int i = 0; i < actualKernelSize; i++)
    {
        kernel[i] /= gaussSum;
    }

    // 横方向のぼかし
    for (int y = 0; y < this->screenSize.y(); y++)
    {
        for (int x = 0; x < this->screenSize.x(); x++)
        {
            Vector3f sum = Vector3f(0, 0, 0);
            for (int lx = -actualKernelSize / 2; lx <= actualKernelSize / 2; lx++)
            {
                int sampleX;
                if (isLoopingX)
                    sampleX = (x + lx) % this->screenSize.x();
                else
                    sampleX = clamp<int>(x + lx, 0, this->screenSize.x() - 1);
                sum += this->SampleColor(sampleX, y) * kernel[lx + actualKernelSize / 2];
            }
            intermediate.PaintPixel(x, y, sum);
        }
    }

    // 縦方向のぼかし
    for (int y = 0; y < this->screenSize.y(); y++)
    {
        for (int x = 0; x < this->screenSize.x(); x++)
        {
            Vector3f sum = Vector3f(0, 0, 0);
            for (int ly = -actualKernelSize / 2; ly <= actualKernelSize / 2; ly++)
            {
                int sampleY;
                if (isLoopingY)
                    sampleY = (y + ly) % this->screenSize.y();
                else
                    sampleY = clamp<int>(y + ly, 0, this->screenSize.y() - 1);
                sum += intermediate.SampleColor(x, sampleY) * kernel[ly + actualKernelSize / 2];
            }
            retval.PaintPixel(x, y, sum);
        }
    }

    return retval;
}

vector<RenderTarget> RenderTarget::GausiannBlurWithDownSample(const vector<DownSampleData> &downSampleData) const
{
    vector<RenderTarget> downSampledBuffers(downSampleData.size());
#pragma omp parallel for
    for (size_t i = 0; i < downSampleData.size(); i++)
    {
        downSampledBuffers[i] = this->DownSample(downSampleData[i].bufferSize)
                                    .GausiannBlur(downSampleData[i].kernelSize)
                                    .UpSample(this->screenSize);
    }
    return downSampledBuffers;
}

vector<RenderTarget> RenderTarget::MakeMipMap(int num) const
{
    vector<RenderTarget> retval;
    int scale = 1;
    for (int i = 0; i < num; i++)
    {
        Vector2i resizedSize = this->screenSize / scale;
        if (resizedSize.x() <= 8 || resizedSize.y() <= 8)
            break;
        retval.push_back(this->DownSample(resizedSize));
        scale *= 2;
    }
    return retval;
}
vector<RenderTarget> RenderTarget::MakeMipMapBluered(int num, int kernelSize) const
{
    vector<RenderTarget> retval;
    int scale = 1;
    retval.push_back(*this);
    for (int i = 1; i < num; i++)
    {
        Vector2i resizedSize = this->screenSize / scale;
        if (resizedSize.x() <= 8 || resizedSize.y() <= 8)
            break;
        retval.push_back(retval[i - 1].GausiannBlur(kernelSize, true).DownSample(resizedSize));
        scale *= 2;
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
    os << "RenderTarget(" << &rt << ")" << "(" << rt.screenSize.x() << "," << rt.screenSize.y() << ")" << endl;
    return os;
}
