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
            unsigned char r = static_cast<unsigned char>(color.x());
            unsigned char g = static_cast<unsigned char>(color.y());
            unsigned char b = static_cast<unsigned char>(color.z());

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
    XEvent event;
    while (true)
    {
        XNextEvent(display, &event);
        if (event.type == KeyPress)
            break; // Exit on key press
    }
}