#pragma once

#include <GLES3/gl3.h>
#include <utils/math.h>

#include <Shader.h>

using namespace engine::utils::math;
using namespace engine::utils::rendering;

namespace engine {
    class GLRenderer {
        public:
            GLRenderer() = default;
            ~GLRenderer();

            bool init();
            void shutdown();

            void beginFrame() {}
            void endFrame();

            void clear(const Color& color);
            void setViewport(int x, int y, int width, int height);

            void drawQuad(const Vector2& position, const Vector2& size, GLuint textureID, const Color& tint = Color(1.0f, 1.0f, 1.0f, 1.0f));
            void drawQuad(const Vector2& position, const Vector2& size, const Vector2& texCoordMin, const Vector2& texCoordMax, GLuint textureID, const Color tint = Color(1.0f, 1.0f, 1.0f, 1.0f));

            void drawRect(const Vector2& position, const Vector2& size, const Color& color);
            void drawLine(const Vector2& start, const Vector2& end, const Color& color);

            void setProjectionMatrix(const float* matrix);
            void setOrthographicProjection(float left, float right, float bottom, float top);

            Shader* getDefaultShader() { return &m_SpriteShader; }

        private:
            bool m_Initialized = false;

            Shader m_SpriteShader;

            // Quad rendering
            GLuint m_QuadVAO = 0;
            GLuint m_QuadVBO = 0;
            GLuint m_QuadEBO = 0;

            // Line rendering
            GLuint m_LineVAO = 0;
            GLuint m_LineVBO = 0;

            std::array<float, 16> m_ProjectionMatrix{};
            std::array<float, 16> m_ViewMatrix{};

            // State caching for performance
            GLuint m_CurrentVAO = 0;
            GLuint m_CurrentTexture = 0;

            bool m_ProjectionDirty = true;
            bool m_ViewDirty = true;

            void createQuadBuffers();
            void createLineBuffers();
            void destroyBuffers();

            void createOrthographicMatrix(float left, float right, float bottom, float top, float* out);
            void createIdentityMatrix(float* out);

            void bindVAO(GLuint vao);
            void bindTexture(GLuint texture);
            void updateUniforms();
    };
}
