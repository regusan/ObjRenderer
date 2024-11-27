#pragma once

// X11とEigenでSuccessシンボルが競合するためこの順序でインクルードすること！！
#include "../header/EigenHeader.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
//

#include <vector>
#include <iostream>
#include "../header/RenderingHeader.hpp"

class X11Display
{
private:
    Display *display;
    Window window;
    GC gc; // Graphics Context
    int screen;
    XImage *xImage;
    int width, height;

    void createWindow(int width, int height);
    void updateWindow(RenderTarget &renderTarget);

public:
    X11Display(int width, int height);
    ~X11Display();

    void show(RenderTarget &renderTarget);
    void waitUntilWindowBreak();
};
