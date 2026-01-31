#pragma once

#include <GLES3/gl3.h>
#include <utils/math.h>

using namespace engine::utils::math;

namespace engine {
    class Texture {
        public:
            Texture(const std::string& workingDir) : m_TextureID(0), m_Width(0), m_Height(0), m_WorkingDir(workingDir) {}
            ~Texture();

            const bool loadTexture(const std::string& fileName, bool linearFiltering = false);

            const bool render(const Vector2& size, const Vector2& position);
            const bool render(const Vector2& size, const Vector2& position, const Vector2& cropSize, const Vector2& cropPosition);

            GLuint getTextureID() const { return m_TextureID; }

            int getWidth() const { return m_Width; }
            int getHeight() const { return m_Height; }

        private:
            std::string m_WorkingDir;
            GLuint m_TextureID;

            int m_Width;
            int m_Height;
    };
}
