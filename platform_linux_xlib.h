#pragma once

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>

#include <string>

struct platform_window_t
{
    Display *display;
    Colormap colormap;
    Window handle;
    Atom delete_message;
    GLXContext context;
};

namespace platform
{
    platform_window_t *create_window(const std::string &title, unsigned int width, unsigned int height)
    {
        Display *display = XOpenDisplay(nullptr);
        if (!display) {
            return nullptr;
        }

        int attrs[] = {
            GLX_RGBA,
            GLX_DOUBLEBUFFER,
            GLX_RED_SIZE, 8,
            GLX_GREEN_SIZE, 8,
            GLX_BLUE_SIZE, 8,
            GLX_ALPHA_SIZE, 8,
            GLX_DEPTH_SIZE, 24,
            GLX_STENCIL_SIZE, 8,
            None
        };

        XVisualInfo *vi = glXChooseVisual(display, 0, attrs);
        if (vi == NULL) {
            return nullptr;
        }

        Window root = DefaultRootWindow(display);
        Colormap colormap = XCreateColormap(display, root, vi->visual, AllocNone);

        XSetWindowAttributes swa = XSetWindowAttributes();
        swa.colormap = colormap;

        Window handle = XCreateWindow(display, root, 0, 0, width, height, 0, vi->depth, InputOutput, vi->visual, CWColormap, &swa);

        // change window title
        XStoreName(display, handle, title.c_str());

        XSelectInput(display, handle, KeyPressMask);

        // register for window close message
        Atom delete_message = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, handle, &delete_message, 1);

        // create OpenGL context

        GLXContext context = glXCreateContext(display, vi, NULL, True);

        XFree(vi);

        glXMakeCurrent(display, handle, context);

        // show window
        XMapWindow(display, handle);

        platform_window_t *window = new platform_window_t;
        window->display = display;
        window->colormap = colormap;
        window->handle = handle;
        window->delete_message = delete_message;
        window->context = context;

        return window;
    }

    void destroy_window(platform_window_t *window)
    {
        glXMakeCurrent(window->display, None, NULL);
        glXDestroyContext(window->display, window->context);

        XDestroyWindow(window->display, window->handle);
        XFreeColormap(window->display, window->colormap);
        XCloseDisplay(window->display);

        window->display = 0;
        window->colormap = 0;
        window->handle = 0;
        window->context = 0;
    }

    bool handle_events(platform_window_t *window)
    {
        if (XPending(window->display) > 0) {
            XEvent event;
            XNextEvent(window->display, &event);

            if (event.xclient.data.l[0] == (long)window->delete_message) {
                return true;
            }

            if (event.type == KeyPress) {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                if (key == XK_Escape || key == XK_q || key == XK_Q) {
                    return true;
                }
            }
        }

        return false;
    }

    void *get_gl_function_pointer(const std::string &name)
    {
        return (void *)glXGetProcAddress((const GLubyte *)name.c_str());
    }

    void swap(platform_window_t *window)
    {
        glXSwapBuffers(window->display, window->handle);
    }
}
