#include "renderer/render_target.h"

#include "platform/logger.h"

namespace Renderer {

namespace {
constexpr const char* kBlitVertexSource = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;
out vec2 vTex;
void main() {
    vTex = aTex;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

constexpr const char* kBlitFragmentSource = R"(
#version 330 core
in vec2 vTex;
out vec4 FragColor;
uniform sampler2D uTexture;
void main() {
    FragColor = texture(uTexture, vTex);
}
)";

GLuint CompileShaderStage(GLenum type, const char* source) {
    const GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &source, nullptr);
    glCompileShader(sh);
    GLint ok = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (ok == 0) {
        char log[512];
        glGetShaderInfoLog(sh, 512, nullptr, log);
        Platform::Logger::Error(std::string("Blit shader compile failed: ") + log);
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}
} // namespace

RenderTarget::~RenderTarget() {
    Destroy();
}

void RenderTarget::Destroy() {
    if (m_blitProgram != 0) {
        glDeleteProgram(m_blitProgram);
        m_blitProgram = 0;
    }
    if (m_quadVbo != 0) {
        glDeleteBuffers(1, &m_quadVbo);
        m_quadVbo = 0;
    }
    if (m_quadVao != 0) {
        glDeleteVertexArrays(1, &m_quadVao);
        m_quadVao = 0;
    }
    if (m_depthRenderbuffer != 0) {
        glDeleteRenderbuffers(1, &m_depthRenderbuffer);
        m_depthRenderbuffer = 0;
    }
    if (m_colorTexture != 0) {
        glDeleteTextures(1, &m_colorTexture);
        m_colorTexture = 0;
    }
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    m_width = 0;
    m_height = 0;
}

bool RenderTarget::Initialize(const int internalWidth, const int internalHeight) {
    Destroy();
    m_width = internalWidth;
    m_height = internalHeight;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    // Nearest filtering is intentional for PSX retro look
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);

    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Platform::Logger::Error("RenderTarget framebuffer incomplete");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        Destroy();
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Fullscreen quad for blit
    constexpr float kQuadVerts[] = {
        // pos     tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
    };
    glGenVertexArrays(1, &m_quadVao);
    glGenBuffers(1, &m_quadVbo);
    glBindVertexArray(m_quadVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVerts), kQuadVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glBindVertexArray(0);

    const GLuint vs = CompileShaderStage(GL_VERTEX_SHADER, kBlitVertexSource);
    const GLuint fs = CompileShaderStage(GL_FRAGMENT_SHADER, kBlitFragmentSource);
    if (vs == 0 || fs == 0) {
        if (vs != 0) glDeleteShader(vs);
        if (fs != 0) glDeleteShader(fs);
        Destroy();
        return false;
    }
    m_blitProgram = glCreateProgram();
    glAttachShader(m_blitProgram, vs);
    glAttachShader(m_blitProgram, fs);
    glLinkProgram(m_blitProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint linked = 0;
    glGetProgramiv(m_blitProgram, GL_LINK_STATUS, &linked);
    if (linked == 0) {
        char log[512];
        glGetProgramInfoLog(m_blitProgram, 512, nullptr, log);
        Platform::Logger::Error(std::string("Blit program link failed: ") + log);
        Destroy();
        return false;
    }

    Platform::Logger::Info("RenderTarget initialized " + std::to_string(m_width) + "x" + std::to_string(m_height));
    return true;
}

void RenderTarget::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
}

void RenderTarget::Unbind(const int windowWidth, const int windowHeight) const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
}

void RenderTarget::BlitToScreen(const int windowWidth, const int windowHeight) const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(m_blitProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glUniform1i(glGetUniformLocation(m_blitProgram, "uTexture"), 0);
    glBindVertexArray(m_quadVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void RenderTarget::Resize(const int internalWidth, const int internalHeight) {
    Initialize(internalWidth, internalHeight);
}

} // namespace Renderer
