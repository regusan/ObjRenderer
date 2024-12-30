cmake_minimum_required(VERSION 3.10)
project(Renderer)

# C++のバージョンを指定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ビルドタイプの設定
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# コンパイルオプション
set(CMAKE_CXX_FLAGS "-Wall -MMD -MP")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mtune=native -march=native -mfpmath=both -DNDEBUG")

# OpenMPの設定
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Eigenの設定（Eigenはヘッダオンリーなのでインクルードパスを指定するだけ）
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# X11の設定
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})

# ソースファイルの検索
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# 実行ファイルの作成
add_executable(Regu3D ${SOURCES})

# リンクライブラリ
target_link_libraries(Regu3D ${X11_LIBRARIES} OpenMP::OpenMP_CXX)
