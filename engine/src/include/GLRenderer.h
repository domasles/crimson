#pragma once

#include <utils/math.h>
#include <utils/rendering.h>

#include <GLES3/gl3.h>

#include <Shader.h>

using namespace engine::utils::math;
using namespace engine::utils::rendering;

namespace engine {
    enum class RenderPass {
        Opaque,  // Textured quads (game objects)
        Debug,   // Gizmo lines and debug visualization
        UI       // UI overlay
    };

    class GLRenderer {
        public:
            GLRenderer() = default;
            ~GLRenderer();

            bool init();
            void shutdown();

            void beginFrame() {}
            void endFrame();

            void beginPass(RenderPass pass);
            void endPass();

            void clear(const Color& color);
            void setViewport(int x, int y, int width, int height);

            void drawQuad(const Vector2& position, const Vector2& size, GLuint textureID, const Color& tint = Color(1.0f, 1.0f, 1.0f, 1.0f), float rotation = 0.0f, const Vector2& scale = Vector2{1.0f, 1.0f});
            void drawQuad(const Vector2& position, const Vector2& size, const Vector2& texCoordMin, const Vector2& texCoordMax, GLuint textureID, const Color tint = Color(1.0f, 1.0f, 1.0f, 1.0f), float rotation = 0.0f, const Vector2& scale = Vector2{1.0f, 1.0f});

            void drawRect(const Vector2& position, const Vector2& size, const Color& color, float rotation = 0.0f);
            void drawLine(const Vector2& start, const Vector2& end, const Color& color);

            void setProjectionMatrix(const float* matrix);
            void setOrthographicProjection(float left, float right, float bottom, float top);

            const float* getProjectionMatrix() const { return m_ProjectionMatrix.data(); }

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

            // State caching for performance
            GLuint m_CurrentVAO = 0;
            GLuint m_CurrentTexture = 0;

            bool m_ProjectionDirty = true;

            // Current render pass
            RenderPass m_CurrentPass = RenderPass::Opaque;

            // Batching system - Quads
            static constexpr size_t MAX_QUADS_PER_BATCH = 1000;
            static constexpr size_t MAX_VERTICES_PER_BATCH = MAX_QUADS_PER_BATCH * 4;
            static constexpr size_t MAX_INDICES_PER_BATCH = MAX_QUADS_PER_BATCH * 6;

            std::vector<float> m_QuadBatchVertices;
            std::vector<uint32_t> m_QuadBatchIndices;

            GLuint m_CurrentBatchTexture = 0;

            size_t m_QuadBatchCount = 0;

            // Batching system - Lines
            static constexpr size_t MAX_LINES_PER_BATCH = 2000;
            static constexpr size_t MAX_LINE_VERTICES_PER_BATCH = MAX_LINES_PER_BATCH * 2;

            std::vector<float> m_LineBatchVertices;

            size_t m_LineBatchCount = 0;

            void createQuadBuffers();
            void createLineBuffers();
            void destroyBuffers();

            void createOrthographicMatrix(float left, float right, float bottom, float top, float* out);
            void createIdentityMatrix(float* out);

            void bindVAO(GLuint vao);
            void bindTexture(GLuint texture);
            void updateUniforms();

            // Batching helpers
            void addQuadToBatch(const Vector2& position, const Vector2& size, const Vector2& texCoordMin, const Vector2& texCoordMax, GLuint textureID, const Color& tint, float rotation = 0.0f, const Vector2& scale = Vector2{1.0f, 1.0f});
            void flushQuadBatch();

            void addLineToBatch(const Vector2& start, const Vector2& end, const Color& color);
            void addRectToBatch(float x, float y, float w, float h, const Color& color, float rotation = 0.0f);

            void flushLineBatch();
    };
}
