#pragma once

#include <glad/glad.h>

namespace Renderer {

class RenderTarget {
public:
    RenderTarget() = default;
    ~RenderTarget();

    RenderTarget(const RenderTarget&) = delete;
    RenderTarget& operator=(const RenderTarget&) = delete;

    bool Initialize(int internalWidth, int internalHeight);
    void Bind() const;
    void Unbind(int windowWidth, int windowHeight) const;
    void BlitToScreen(int windowWidth, int windowHeight) const;
    void Resize(int internalWidth, int internalHeight);

    GLuint GetColorTexture() const { return m_colorTexture; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    bool IsValid() const { return m_fbo != 0; }

private:
    void Destroy();

    GLuint m_fbo{0};
    GLuint m_colorTexture{0};
    GLuint m_depthRenderbuffer{0};
    GLuint m_quadVao{0};
    GLuint m_quadVbo{0};
    GLuint m_blitProgram{0};
    int m_width{0};
    int m_height{0};
};

} // namespace Renderer
