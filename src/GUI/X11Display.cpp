#include "X11Display.hpp"

X11Display::X11Display(int width, int height)
    : width(width), height(height)
{
    display = XOpenDisplay(nullptr);
    if (!display)
    {
        std::cerr << "Unable to open X display!" << std::endl;
        exit(1);
    }

    screen = DefaultScreen(display);
    createWindow(width, height);
}

X11Display::~X11Display()
{
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void X11Display::createWindow(int width, int height)
{
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, width, height, 1,
                                 BlackPixel(display, screen), WhitePixel(display, screen));
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    gc = XCreateGC(display, window, 0, nullptr);
    XMapWindow(display, window);

    this->WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1);

    // Create an XImage to represent the pixel data
    xImage = XCreateImage(display, DefaultVisual(display, screen), DefaultDepth(display, screen),
                          ZPixmap, 0, nullptr, width, height, 32, 0);
    xImage->data = (char *)malloc(xImage->bytes_per_line * height);
}

void X11Display::updateWindow(RenderTarget &renderTarget)
{
    // Convert RenderTarget data to XImage data
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Vector3f color = renderTarget.SampleColor(x, y);
            unsigned char r = static_cast<unsigned char>(clamp<float>(color.x() * 255, 0, 255));
            unsigned char g = static_cast<unsigned char>(clamp<float>(color.y() * 255, 0, 255));
            unsigned char b = static_cast<unsigned char>(clamp<float>(color.z() * 255, 0, 255));

            // Set pixel at (x, y)
            unsigned long pixel = (r << 16) | (g << 8) | b; // RGB -> 32-bit value
            XPutPixel(xImage, x, y, pixel);
        }
    }

    // Draw the image onto the window
    XPutImage(display, window, gc, xImage, 0, 0, 0, 0, width, height);
}

void X11Display::show(RenderTarget &renderTarget)
{
    updateWindow(renderTarget);
    XFlush(display); // Ensure the X11 commands are processed immediately
}

void X11Display::waitUntilWindowBreak()
{
    while (true)
    {
        if (this->GetEvent().type == KeyPress)
            break; // Exit on key press
    }
}

XEvent X11Display::GetEvent()
{
    XEvent event;
    XNextEvent(display, &event);
    return event;
}
Display *X11Display::GetDisplay()
{
    return this->display;
}

Vector2i X11Display::GetMousePos()
{
    Window root, child;
    int rootX, rootY, winX, winY;
    unsigned int mask;

    // XQueryPointerを使用してマウス位置を取得
    XQueryPointer(display, window, &root, &child, &rootX, &rootY, &winX, &winY, &mask);

    // ウィンドウ内の相対位置（winX, winY）を返す
    return Vector2i(winX, winY);
}