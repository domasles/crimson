#include <pch.h>

#include <utils/logger.h>
#include <utils/filesystem.h>

#include <Shader.h>

using namespace engine::utils::logger;
using namespace engine::utils::filesystem;

namespace engine {
    Shader::~Shader() {
        cleanup();
    }

    Shader::Shader(Shader&& other) noexcept : m_ProgramID(other.m_ProgramID), m_UniformCache(std::move(other.m_UniformCache)) {
        other.m_ProgramID = 0;
    }

    Shader& Shader::operator=(Shader&& other) noexcept {
        if (this != &other) {
            cleanup();

            m_ProgramID = other.m_ProgramID;
            m_UniformCache = std::move(other.m_UniformCache);

            other.m_ProgramID = 0;
        }

        return *this;
    }

    bool Shader::loadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string vertexSource = readFileToString(getGamePath() + "/" + vertexPath);
        std::string fragmentSource = readFileToString(getGamePath() + "/" + fragmentPath);

        if (vertexSource.empty() || fragmentSource.empty()) {
            Logger::engine_error("Failed to read shader files: {} and {}", vertexPath, fragmentPath);
            return false;
        }

        return loadFromSource(vertexSource, fragmentSource);
    }

    bool Shader::loadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
        cleanup();

        GLuint vertexShader = 0;
        GLuint fragmentShader = 0;

        if (!compileShader(vertexShader, GL_VERTEX_SHADER, vertexSource)) {
            Logger::engine_error("Failed to compile vertex shader");
            return false;
        }

        if (!compileShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentSource)) {
            Logger::engine_error("Failed to compile fragment shader");
            glDeleteShader(vertexShader);

            return false;
        }

        if (!linkProgram(vertexShader, fragmentShader)) {
            Logger::engine_error("Failed to link shader program");

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            return false;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        ENGINE_LOG_INIT("Shader");
        return true;
    }

    void Shader::use() const {
        if (m_ProgramID != 0) glUseProgram(m_ProgramID);
    }

    void Shader::unbind() const {
        glUseProgram(0);
    }

    void Shader::setInt(const std::string& name, int value) {
        glUniform1i(getUniformLocation(name), value);
    }

    void Shader::setFloat(const std::string& name, float value) {
        glUniform1f(getUniformLocation(name), value);
    }

    void Shader::setVec2(const std::string& name, const Vector2& value) {
        glUniform2f(getUniformLocation(name), value.getX(), value.getY());
    }

    void Shader::setVec3(const std::string& name, float x, float y, float z) {
        glUniform3f(getUniformLocation(name), x, y, z);
    }

    void Shader::setVec4(const std::string& name, float x, float y, float z, float w) {
        glUniform4f(getUniformLocation(name), x, y, z, w);
    }

    void Shader::setMat4(const std::string& name, const float* value) {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value);
    }

    bool Shader::compileShader(GLuint& shader, GLenum type, const std::string& source) {
        shader = glCreateShader(type);
        const char* src = source.c_str();

        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            GLchar infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            Logger::engine_error("Shader compilation error ({}): {}", type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT", infoLog);

            return false;
        }

        return true;
    }

    bool Shader::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
        m_ProgramID = glCreateProgram();

        glAttachShader(m_ProgramID, vertexShader);
        glAttachShader(m_ProgramID, fragmentShader);
        glLinkProgram(m_ProgramID);

        GLint success;
        glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);

        if (!success) {
            GLchar infoLog[512];

            glGetProgramInfoLog(m_ProgramID, 512, nullptr, infoLog);
            Logger::engine_error("Shader program linking error: {}", infoLog);

            glDeleteProgram(m_ProgramID);
            m_ProgramID = 0;

            return false;
        }

        return true;
    }

    GLint Shader::getUniformLocation(const std::string& name) const {
        auto it = m_UniformCache.find(name);
        if (it != m_UniformCache.end()) return it->second;

        GLint location = glGetUniformLocation(m_ProgramID, name.c_str());

        if (location == -1) {
            Logger::engine_warn("Uniform '{}' not found in shader", name);
        }

        m_UniformCache[name] = location;
        return location;
    }

    void Shader::cleanup() {
        if (m_ProgramID != 0) {
            glDeleteProgram(m_ProgramID);
            m_ProgramID = 0;
        }

        m_UniformCache.clear();
    }
}
