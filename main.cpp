#include <cstdio>

#if defined(_WIN32)
#include "platform_windows.h"
#elif defined(LINUX)
#include "platform_linux_xlib.h"
#else
#error Unsupported platform
#endif

#include <set>
#include <utility>
#include <string>
#include <vector>

PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;

bool Initialize()
{
    // shaders

    std::vector<std::pair<GLenum, std::string>> shaders = {
        {
            GL_VERTEX_SHADER,
            "#version 130\n"
            "in vec4 position;\n"
            "out vec2 uv;\n"
            "\n"
            "void main() {\n"
            "  gl_Position = position;\n"
            "  uv = (position.xy + 1.0) / 2.0;\n"
            "}\n"
        },
        {
            GL_FRAGMENT_SHADER,
            "#version 130\n"
            "in vec2 uv;\n"
            "out vec4 color;\n"
            "\n"
            "void main() {\n"
            "  color = vec4(uv, 0.0, 1.0);\n"
            "}\n"
        },
    };

    GLuint program = glCreateProgram();

    for (const auto &s: shaders) {
        GLenum type = s.first;
        const std::string &source = s.second;

        const GLchar *src = source.c_str();

        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);

        glCompileShader(shader);

        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

            if (length > 1) {
                std::string log(length, '\0');
                glGetShaderInfoLog(shader, length, &length, &log[0]);
                printf("Shader compile failed:\n%s\n", log.c_str());
            } else {
                printf("Shader compile failed.\n");
            }

            return false;
        }

        glAttachShader(program, shader);
    }

    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        if (length > 1) {
            std::string log(length, '\0');
            glGetProgramInfoLog(program, length, &length, &log[0]);
            printf("Program link failed:\n%s", log.c_str());
        } else {
            printf("Program link failed.\n");
        }

        return false;
    }

    // vertex buffer

    float vertices[] = {
        -1.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f
    };

    GLuint vb;
    glGenBuffers(1, &vb);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // initial state
    glUseProgram(program);

    GLint position = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(position);

    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 0, 0);

    return true;
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main()
{
    const unsigned int client_width = 512;
    const unsigned int client_height = 512;

    platform_window_t *window = platform::create_window("Triangle", client_width, client_height);
    if (!window) {
        printf("Failed to create window.\n");
        return 1;
    }

    printf("GL_VENDOR: %s\n", glGetString(GL_VENDOR));
    printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
    printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));

    // initialize OpenGL function pointers

    glCreateProgram = (PFNGLCREATEPROGRAMPROC)platform::get_gl_function_pointer("glCreateProgram");
    glCreateShader = (PFNGLCREATESHADERPROC)platform::get_gl_function_pointer("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)platform::get_gl_function_pointer("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)platform::get_gl_function_pointer("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)platform::get_gl_function_pointer("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)platform::get_gl_function_pointer("glGetShaderInfoLog");
    glAttachShader = (PFNGLATTACHSHADERPROC)platform::get_gl_function_pointer("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)platform::get_gl_function_pointer("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)platform::get_gl_function_pointer("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)platform::get_gl_function_pointer("glGetProgramInfoLog");
    glUseProgram = (PFNGLUSEPROGRAMPROC)platform::get_gl_function_pointer("glUseProgram");
    glGenBuffers = (PFNGLGENBUFFERSPROC)platform::get_gl_function_pointer("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)platform::get_gl_function_pointer("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)platform::get_gl_function_pointer("glBufferData");
    glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)platform::get_gl_function_pointer("glGetAttribLocation");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)platform::get_gl_function_pointer("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)platform::get_gl_function_pointer("glVertexAttribPointer");

    if (!Initialize()) {
        printf("Scene initialization failed.\n");
        return 1;
    }

    while (true) {
        bool quit = platform::handle_events(window);
        if (quit) {
            break;
        }

        Render();

        platform::swap(window);
    }

    platform::destroy_window(window);

    delete window;

    return 0;
}
