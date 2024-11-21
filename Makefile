# コンパイラとコンパイルフラグの設定
CXX = g++
CXXFLAGS = -Wall -I /usr/local/include/eigen3 -g -O0

# デフォルトで使用するC++のバージョン
CXX_VERSION = c++17  # ここでC++バージョンを指定（例: c++17, c++20）

# C++バージョン指定
CXXFLAGS += -std=$(CXX_VERSION)

# ソースコードファイル
#SRC = src/main.cpp src/RenderTarget.cpp src/Model.cpp src/shader.cpp  src/RenderingLibrary.cpp src/Math3D.cpp# 必要に応じてソースファイルを追加
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:.cpp=.o)

# 実行ファイル名
TARGET = renderer  # 出力される実行ファイル名

# リンクフラグ
LDFLAGS = -lm

# デフォルトターゲット（実行ファイルを作成する）
all: $(TARGET)

# 実行ファイルを作成
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# ソースファイルからオブジェクトファイルを作成
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# クリーンターゲット（ビルドしたファイルを削除）
clean:
	rm -f $(OBJ) $(TARGET)

# 依存関係のターゲットを生成（必要に応じて追加）
