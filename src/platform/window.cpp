#include "platform/window.h"

#include "foundation/logger.h"

#include <glad/glad.h>

namespace Platform {

Window::Window(const int width, const int height, const std::string& title)
    : m_window(nullptr)
    , m_glContext(nullptr)
    , m_width(width)
    , m_height(height)
    , m_shouldClose(false) {
    if (!InitializeVideo()) {
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (m_window == nullptr) {
        Foundation::Logger::Error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
        return;
    }

    if (!CreateGlContext()) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        return;
    }

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        Foundation::Logger::Error("Failed to initialize GLAD");
        SDL_GL_DeleteContext(m_glContext);
        m_glContext = nullptr;
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        return;
    }

    Foundation::Logger::Info(std::string("OpenGL ") + reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    Foundation::Logger::Info(std::string("GLSL ") + reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    SDL_GL_SetSwapInterval(1);
}

Window::~Window() {
    if (m_glContext != nullptr) {
        SDL_GL_DeleteContext(m_glContext);
        m_glContext = nullptr;
    }
    if (m_window != nullptr) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    SDL_Quit();
}

bool Window::InitializeVideo() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        Foundation::Logger::Error(std::string("SDL_Init failed: ") + SDL_GetError());
        return false;
    }
    return true;
}

bool Window::CreateGlContext() {
    m_glContext = SDL_GL_CreateContext(m_window);
    if (m_glContext == nullptr) {
        Foundation::Logger::Error(std::string("SDL_GL_CreateContext failed: ") + SDL_GetError());
        return false;
    }
    if (SDL_GL_MakeCurrent(m_window, m_glContext) != 0) {
        Foundation::Logger::Error(std::string("SDL_GL_MakeCurrent failed: ") + SDL_GetError());
        return false;
    }
    return true;
}

void Window::PollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            m_shouldClose = true;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            m_shouldClose = true;
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            m_width = event.window.data1;
            m_height = event.window.data2;
        }
    }
}

void Window::SwapBuffers() const {
    if (m_window != nullptr) {
        SDL_GL_SwapWindow(m_window);
    }
}

} // namespace Platform
