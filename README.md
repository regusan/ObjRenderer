# ObjRenderer

# DEMO
![out_beauty](https://github.com/user-attachments/assets/c47dedca-a889-4085-80e2-755ae3c0e3a7)
![image](https://github.com/user-attachments/assets/794d9e64-7604-422e-9b6f-572ae8c7bcfc)


# Features
### Rendering Features
- Smooth Shading
- PPM Texture
- Deferred Rendering
- Post Processing

### IO
- Obj File Reading
- PPM Image Output
- X11 Window Output
- Customizable Vertex Shader
- Customizable Pixel Shader
- Customizable Deffered lighting pass shader
- Customizable Post Process Shader
# Control
| Control | Key |
| ---- | ---- |
| Camera Rotation | Mouse Movement |
| Cameraboom length | up & down button |
| apply config changes | enter button |

# Enviroment
* Ubuntu(WSL2)
# Requirement
* make
* CMake
* g++(C++17)
* Eigen

# Installation
```bash
sudo apt install make -y
sudo apt install g++ -y
sudo apt install cmake -y
sudo apt install libeigen3-dev -y 
sudo apt install x11-apps -y
sudo apt install libx11-dev -y
sudo apt install libopencv-dev -y

git clone https://github.com/regusan/ObjRenderer.git
cd ObjRenderer
```

# Usage
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
./Regu3D models/room.obj
```

# Customize
## VertexShader & PixcelShader
1.  Define your shader
```cpp
/// @brief 頂点シェーダーの例
/// @param in 入力データ格納構造体
/// @return 出力データ格納構造体
inline const VertOutputStandard YourOwnVertShader(const VertInputStandard &in)
{
  //Write own vertex transform 
}

/// @brief ピクセルシェーダーの例
/// @param in 入力データ格納構造体
/// @return 出力データ格納構造体
inline const PixcelOutputStandard YourOwnPixcelShader(const PixcelInputStandard &in)
{
  //Write own pixcel shader
}
```
2. Set geometry pass argument
```cpp
RenderingPipeline::Deffered::ExecGeometryPass(model, in, gb, YourOwnVertShader, YourOwnPixcelShader);
```

## Second Pass(Deffered Lighting Pass)
1.  Define your light pass shader
```cpp
/// @brief ライトパスシェーダー
/// @param gbuffers
/// @param environment
/// @param x
/// @param y
/// @return 該当ピクセルの色
inline const Vector3f YourOwnLightPassShader(GBuffers &gbuffers, RenderingEnvironmentParameters &environment, int x, int y)
{
  //define your light pass shader
  return color;
}
```
2. Set lighting pass argment
```cpp
RenderingPass::ExecLightingPass(gb, YourOwnLightPassShader, in.environment);
```

# License
This is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).
