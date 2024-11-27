# コンパイラとリンク設定
CXX = g++
LDFLAGS = -lm -I /usr/local/include/eigen3 -lX11

# デフォルトで使用するC++のバージョン
CXX_VERSION = c++17  # ここでC++バージョンを指定

# コンパイルフラグ
CXXFLAGS_COMMON = -Wall -std=$(CXX_VERSION)
CXXFLAGS_DEBUG = -g -O0
CXXFLAGS_RELEASE = -O3 -mtune=native -march=native -mfpmath=both -DNDEBUG

# ソースコードファイルとオブジェクトファイル
SRC_DIR = src
SRC = $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJ = $(SRC:.cpp=.o)

# 実行ファイル名
DEBUG_TARGET = renderer_debug.out
RELEASE_TARGET = renderer_release.out

# デフォルトターゲット
all: debug release

# デバッグビルド
debug: CXXFLAGS = $(CXXFLAGS_COMMON) $(CXXFLAGS_DEBUG)
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# リリースビルド
release: CXXFLAGS = $(CXXFLAGS_COMMON) $(CXXFLAGS_RELEASE)
release: $(RELEASE_TARGET)

$(RELEASE_TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# ソースファイルからオブジェクトファイルを作成
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# クリーンターゲット（ビルドしたファイルを削除）
clean:
	rm -f $(OBJ) $(DEBUG_TARGET) $(RELEASE_TARGET)

# 必要に応じて依存関係のターゲットを追加
