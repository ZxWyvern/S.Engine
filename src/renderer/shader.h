#pragma once

#include <string>
#include <glad/glad.h>

namespace Renderer {

class Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    void Bind() const;
    void Unbind() const;
    bool IsValid() const { return m_programId != 0; }
    GLuint GetProgramId() const { return m_programId; }

    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;
    void SetVec3(const std::string& name, const float* values) const;
    void SetMat4(const std::string& name, const float* matrix) const;

private:
    static std::string ReadFile(const std::string& path);
    static GLuint CompileShader(GLenum type, const std::string& source);
    static bool CheckCompileStatus(GLuint shader, std::string& outLog);
    static bool CheckLinkStatus(GLuint program, std::string& outLog);
    GLint GetUniformLocation(const std::string& name) const;
    void Destroy();

    GLuint m_programId{0};
};

} // namespace Renderer
