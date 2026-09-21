#pragma once

#include <SDL.h>
#include <string>

namespace Platform {

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool ShouldClose() const { return m_shouldClose; }
    void PollEvents();
    void SwapBuffers() const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    SDL_Window* GetSdlWindow() const { return m_window; }
    bool IsValid() const { return m_window != nullptr; }

private:
    bool InitializeVideo();
    bool CreateGlContext();

    SDL_Window* m_window;
    SDL_GLContext m_glContext;
    int m_width;
    int m_height;
    bool m_shouldClose;
};

} // namespace Platform
