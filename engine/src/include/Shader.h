#pragma once

#include <GLES3/gl3.h>
#include <utils/math.h>

using namespace engine::utils::math;

namespace engine {
    class Shader {
        public:
            Shader() = default;
            ~Shader();

            // Disable copy, enable move
            Shader(const Shader&) = delete;
            Shader& operator=(const Shader&) = delete;

            Shader(Shader&& other) noexcept;
            Shader& operator=(Shader&& other) noexcept;

            bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
            bool loadFromSource(const std::string& vertexSource, const std::string& fragmentSource);

            void use() const;
            void unbind() const;

            // Uniform setters
            void setInt(const std::string& name, int value);
            void setFloat(const std::string& name, float value);
            void setVec2(const std::string& name, const Vector2& value);
            void setVec3(const std::string& name, float x, float y, float z);
            void setVec4(const std::string& name, float x, float y, float z, float w);
            void setMat4(const std::string& name, const float* value);

            GLuint getProgramID() const { return m_ProgramID; }
            bool isValid() const { return m_ProgramID != 0; }

        private:
            GLuint m_ProgramID = 0;
            mutable std::unordered_map<std::string, GLint> m_UniformCache;

            bool compileShader(GLuint& shader, GLenum type, const std::string& source);
            bool linkProgram(GLuint vertexShader, GLuint fragmentShader);
            GLint getUniformLocation(const std::string& name) const;

            void cleanup();
    };
}
