# ObjRenderer

# DEMO
![image](https://github.com/user-attachments/assets/b3aa6e2d-22b5-4974-82d3-30aec1607e81)

# Features
### レンダリング機能
- スムースシェーディング
### IO
- Objファイル読み取り
- PPM画像出力
- X11ウィンドウ出力
- 可換頂点シェーダー
- 可換ピクセルシェーダー


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

# Note
Under Construction

# License
This is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).
