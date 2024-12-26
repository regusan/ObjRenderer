# ObjRenderer

# DEMO
![image](https://github.com/user-attachments/assets/6b595a14-b87a-4f93-9029-b1fc26f5975a)
![image](https://github.com/user-attachments/assets/f9d97b65-8f6a-41e0-95df-8e3ba060b247)

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
* g++(C++17)
* Eigen

# Installation
```bash
apt install make
apt install g++
apt install libeigen3-dev
apt install x11-apps

git clone https://github.com/regusan/ObjRenderer.git
cd ObjRenderer
```

# Usage
```bash
make release
./renderer_release.out models/room.obj
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
