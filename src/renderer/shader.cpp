#include "renderer/shader.h"

#include "platform/logger.h"

#include <fstream>
#include <sstream>

namespace Renderer {

Shader::~Shader() {
    Destroy();
}

Shader::Shader(Shader&& other) noexcept
    : m_programId(other.m_programId) {
    other.m_programId = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        Destroy();
        m_programId = other.m_programId;
        other.m_programId = 0;
    }
    return *this;
}

void Shader::Destroy() {
    if (m_programId != 0) {
        glDeleteProgram(m_programId);
        m_programId = 0;
    }
}

std::string Shader::ReadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        Platform::Logger::Error("Failed to open shader file: " + path);
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::CompileShader(const GLenum type, const std::string& source) {
    const GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    std::string log;
    if (!CheckCompileStatus(shader, log)) {
        Platform::Logger::Error("Shader compile failed: " + log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::CheckCompileStatus(const GLuint shader, std::string& outLog) {
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        outLog.resize(static_cast<size_t>(length));
        glGetShaderInfoLog(shader, length, nullptr, outLog.data());
        return false;
    }
    return true;
}

bool Shader::CheckLinkStatus(const GLuint program, std::string& outLog) {
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == 0) {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        outLog.resize(static_cast<size_t>(length));
        glGetProgramInfoLog(program, length, nullptr, outLog.data());
        return false;
    }
    return true;
}

bool Shader::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    const std::string vertSource = ReadFile(vertexPath);
    const std::string fragSource = ReadFile(fragmentPath);
    if (vertSource.empty() || fragSource.empty()) {
        Platform::Logger::Error("Shader source empty, aborting load");
        return false;
    }
    return LoadFromSource(vertSource, fragSource);
}

bool Shader::LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
    Destroy();

    const GLuint vertShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    if (vertShader == 0) {
        return false;
    }
    const GLuint fragShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragShader == 0) {
        glDeleteShader(vertShader);
        return false;
    }

    m_programId = glCreateProgram();
    glAttachShader(m_programId, vertShader);
    glAttachShader(m_programId, fragShader);
    glLinkProgram(m_programId);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    std::string log;
    if (!CheckLinkStatus(m_programId, log)) {
        Platform::Logger::Error("Shader link failed: " + log);
        glDeleteProgram(m_programId);
        m_programId = 0;
        return false;
    }

    return true;
}

void Shader::Bind() const {
    if (m_programId != 0) {
        glUseProgram(m_programId);
    }
}

void Shader::Unbind() const {
    glUseProgram(0);
}

GLint Shader::GetUniformLocation(const std::string& name) const {
    if (m_programId == 0) {
        return -1;
    }
    return glGetUniformLocation(m_programId, name.c_str());
}

void Shader::SetInt(const std::string& name, const int value) const {
    const GLint loc = GetUniformLocation(name);
    if (loc != -1) {
        glUniform1i(loc, value);
    }
}

void Shader::SetFloat(const std::string& name, const float value) const {
    const GLint loc = GetUniformLocation(name);
    if (loc != -1) {
        glUniform1f(loc, value);
    }
}

void Shader::SetVec3(const std::string& name, const float x, const float y, const float z) const {
    const GLint loc = GetUniformLocation(name);
    if (loc != -1) {
        glUniform3f(loc, x, y, z);
    }
}

void Shader::SetVec3(const std::string& name, const float* values) const {
    const GLint loc = GetUniformLocation(name);
    if (loc != -1) {
        glUniform3fv(loc, 1, values);
    }
}

void Shader::SetMat4(const std::string& name, const float* matrix) const {
    const GLint loc = GetUniformLocation(name);
    if (loc != -1) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, matrix);
    }
}

} // namespace Renderer
