#pragma once
#include "../BuildSetting.hpp"

#ifdef TARGET_X11BACKEND
//  X11とEigenでSuccessシンボルが競合するためこの順序でインクルードすること！！
#include "../header/EigenHeader.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xfixes.h>

#include <vector>
#include <iostream>

class RenderTarget;

/// @brief X11でのウィンドウ表示のためのクラス
class X11Display
{
private:
    GC gc; // Graphics Context
    int screen;
    XImage *xImage;
    int width, height;

    void createWindow(int width, int height);

    void updateWindow(RenderTarget &renderTarget);

public:
    Display *display = nullptr;
    Window window = 0x0;
    X11Display(int width, int height);
    ~X11Display();

    /// @brief WIndow削除時のコールバック用
    Atom WM_DELETE_WINDOW;

    /// @brief RenderTargetを表示
    /// @param renderTarget
    void show(RenderTarget &renderTarget);
    void show(RenderTarget &&renderTarget);
    /// @brief Windowが破棄されるまで待機
    void waitUntilWindowBreak();
    XEvent GetEvent();
    Display *GetDisplay();
    Vector2i GetMousePos();
    void X11ShowMouseCursor(const bool show);
    void SetMousePos(Vector2i pos);

    void Resize(Vector2i size);
};
#endif