#include <pch.h>

#include <utils/logger.h>
#include <utils/rendering.h>

#include <rendering/RenderConstants.h>

#include <GLRenderer.h>

using namespace engine::rendering;
using namespace engine::utils::logger;
using namespace engine::utils::rendering;

namespace engine {
    GLRenderer::~GLRenderer() {
        shutdown();
    }

    bool GLRenderer::init() {
        if (m_Initialized) {
            Logger::engine_warn("GLRenderer already initialized");
            return true;
        }

        #ifdef ENGINE_PLATFORM_WINDOWS
            if (!gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress)) {
                Logger::engine_error("Failed to initialize GLAD");
                return false;
            }
        #endif

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        if (!m_SpriteShader.loadFromFile("shaders/engine/sprite.vert", "shaders/engine/sprite.frag")) {
            Logger::engine_error("Failed to load sprite shader");
            return false;
        }

        createQuadBuffers();
        createLineBuffers();

        createIdentityMatrix(m_ProjectionMatrix.data());

        m_Initialized = true;
        ENGINE_LOG_INIT("OpenGL ES 3.0 Renderer");

        return true;
    }

    void GLRenderer::shutdown() {
        if (!m_Initialized) {
            return;
        }

        destroyBuffers();
        m_Initialized = false;
    }

    void GLRenderer::endFrame() {
        glFlush();
    }

    void GLRenderer::beginPass(RenderPass pass) {
        if (pass != m_CurrentPass) {
            endPass();
        }

        m_CurrentPass = pass;
    }

    void GLRenderer::endPass() {
        switch (m_CurrentPass) {
            case RenderPass::Opaque:
                flushQuadBatch();
                break;
            case RenderPass::Debug:
                flushLineBatch();
                break;
        }
    }

    void GLRenderer::clear(const Color& color) {
        flushQuadBatch();
        flushLineBatch();

        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void GLRenderer::setViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
    }

    void GLRenderer::drawQuad(const Vector2& position, const Vector2& size, GLuint textureID, const Color& tint) {
        drawQuad(position, size, {0.0f, 0.0f}, {1.0f, 1.0f}, textureID, tint);
    }

    void GLRenderer::drawQuad(const Vector2& position, const Vector2& size, const Vector2& texCoordMin, const Vector2& texCoordMax, GLuint textureID, const Color tint) {
        addQuadToBatch(position, size, texCoordMin, texCoordMax, textureID, tint);
    }

    void GLRenderer::drawRect(const Vector2& position, const Vector2& size, const Color& color) {
        float x = position.getX();
        float y = position.getY();
        float w = size.getX();
        float h = size.getY();

        // Add line vertices to batch
        addLineToBatch({x,     y    }, {x + w, y    }, color); // Bottom
        addLineToBatch({x + w, y    }, {x + w, y + h}, color); // Right
        addLineToBatch({x + w, y + h}, {x,     y + h}, color); // Top
        addLineToBatch({x,     y + h}, {x,     y    }, color); // Left
    }

    void GLRenderer::drawLine(const Vector2& start, const Vector2& end, const Color& color) {
        addLineToBatch(start, end, color);
    }

    void GLRenderer::setProjectionMatrix(const float* matrix) {
        flushQuadBatch();
        flushLineBatch();

        std::copy(matrix, matrix + 16, m_ProjectionMatrix.begin());
        m_ProjectionDirty = true;
    }

    void GLRenderer::setOrthographicProjection(float left, float right, float bottom, float top) {
        flushQuadBatch();
        flushLineBatch();

        createOrthographicMatrix(left, right, bottom, top, m_ProjectionMatrix.data());
        m_ProjectionDirty = true;
    }

    void GLRenderer::createQuadBuffers() {
        auto vertices = verticesToFloatArray(UNIT_QUAD_VERTICES);

        glGenVertexArrays(1, &m_QuadVAO);
        glGenBuffers(1, &m_QuadVBO);
        glGenBuffers(1, &m_QuadEBO);

        glBindVertexArray(m_QuadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);

        // Pre-allocate buffer to maximum batch size
        size_t maxVertexBytes = MAX_QUADS_PER_BATCH * 4 * 8 * sizeof(float); // 4 verts, 8 floats each
        glBufferData(GL_ARRAY_BUFFER, maxVertexBytes, nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadEBO);

        // Pre-allocate index buffer to maximum batch size
        size_t maxIndexBytes = MAX_QUADS_PER_BATCH * 6 * sizeof(uint32_t); // 6 indices per quad
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxIndexBytes, nullptr, GL_DYNAMIC_DRAW);

        // Pre-reserve CPU vector capacity to avoid reallocations
        m_QuadBatchVertices.reserve(MAX_QUADS_PER_BATCH * 4 * 8); // 32 floats per quad
        m_QuadBatchIndices.reserve(MAX_QUADS_PER_BATCH * 6);      // 6 indices per quad

        // Position attribute (location 0)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VERTEX2D_SIZE, reinterpret_cast<void*>(VERTEX2D_POSITION_OFFSET));
        glEnableVertexAttribArray(0);

        // TexCoord attribute (location 1)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX2D_SIZE, reinterpret_cast<void*>(VERTEX2D_TEXCOORD_OFFSET));
        glEnableVertexAttribArray(1);

        // Color attribute (location 2)
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, VERTEX2D_SIZE, reinterpret_cast<void*>(VERTEX2D_COLOR_OFFSET));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void GLRenderer::createLineBuffers() {
        glGenVertexArrays(1, &m_LineVAO);
        glGenBuffers(1, &m_LineVBO);

        glBindVertexArray(m_LineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);

        // Pre-allocate buffer to maximum batch size
        size_t maxLineBytes = MAX_LINES_PER_BATCH * 2 * 6 * sizeof(float); // 2 verts, 6 floats each
        glBufferData(GL_ARRAY_BUFFER, maxLineBytes, nullptr, GL_DYNAMIC_DRAW);

        // Pre-reserve CPU vector capacity to avoid reallocations
        m_LineBatchVertices.reserve(MAX_LINES_PER_BATCH * 2 * 6); // 12 floats per line

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute (position 2, not 1, since we don't have texcoords for lines)
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void GLRenderer::destroyBuffers() {
        if (m_QuadVAO != 0) {
            glDeleteVertexArrays(1, &m_QuadVAO);
            m_QuadVAO = 0;
        }

        if (m_QuadVBO != 0) {
            glDeleteBuffers(1, &m_QuadVBO);
            m_QuadVBO = 0;
        }

        if (m_QuadEBO != 0) {
            glDeleteBuffers(1, &m_QuadEBO);
            m_QuadEBO = 0;
        }

        if (m_LineVAO != 0) {
            glDeleteVertexArrays(1, &m_LineVAO);
            m_LineVAO = 0;
        }

        if (m_LineVBO != 0) {
            glDeleteBuffers(1, &m_LineVBO);
            m_LineVBO = 0;
        }
    }

    void GLRenderer::createOrthographicMatrix(float left, float right, float bottom, float top, float* out) {
        std::fill(out, out + 16, 0.0f);

        out[0] = 2.0f / (right - left);
        out[5] = 2.0f / (top - bottom);
        out[10] = -1.0f;
        out[12] = -(right + left) / (right - left);
        out[13] = -(top + bottom) / (top - bottom);
        out[15] = 1.0f;
    }

    void GLRenderer::createIdentityMatrix(float* out) {
        std::fill(out, out + 16, 0.0f);
        out[0] = out[5] = out[10] = out[15] = 1.0f;
    }

    void GLRenderer::bindVAO(GLuint vao) {
        if (m_CurrentVAO != vao) {
            glBindVertexArray(vao);
            m_CurrentVAO = vao;
        }
    }

    void GLRenderer::bindTexture(GLuint texture) {
        if (m_CurrentTexture != texture) {
            if (texture != 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture);
            } else {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            m_CurrentTexture = texture;
        }
    }

    void GLRenderer::updateUniforms() {
        if (m_ProjectionDirty) {
            m_SpriteShader.setMat4("u_Projection", m_ProjectionMatrix.data());
            m_ProjectionDirty = false;
        }
    }

    void GLRenderer::addQuadToBatch(const Vector2& position, const Vector2& size, const Vector2& texCoordMin, const Vector2& texCoordMax, GLuint textureID, const Color& tint) {
        // Flush if texture changes or batch is full
        if ((m_CurrentBatchTexture != textureID && m_QuadBatchCount > 0) || m_QuadBatchCount >= MAX_QUADS_PER_BATCH) {
            flushQuadBatch();
        }

        m_CurrentBatchTexture = textureID;

        // Build quad vertices using SCALED coordinates
        float x = position.getX();
        float y = position.getY();
        float w = size.getX();
        float h = size.getY();

        std::array<Vertex2D, 4> quadVertices = {{
            Vertex2D({x,     y    }, {texCoordMin.getX(), texCoordMin.getY()}, tint),
            Vertex2D({x + w, y    }, {texCoordMax.getX(), texCoordMin.getY()}, tint),
            Vertex2D({x + w, y + h}, {texCoordMax.getX(), texCoordMax.getY()}, tint),
            Vertex2D({x,     y + h}, {texCoordMin.getX(), texCoordMax.getY()}, tint),
        }};

        // Convert to flat array and append to batch
        auto vertices = verticesToFloatArray(quadVertices);
        m_QuadBatchVertices.insert(m_QuadBatchVertices.end(), vertices.begin(), vertices.end());

        uint32_t baseVertex = static_cast<uint32_t>(m_QuadBatchCount * 4);

        std::array<uint32_t, 6> quadIndices = {{
            baseVertex + 0, baseVertex + 1, baseVertex + 2,
            baseVertex + 2, baseVertex + 3, baseVertex + 0
        }};

        m_QuadBatchIndices.insert(m_QuadBatchIndices.end(), quadIndices.begin(), quadIndices.end());

        m_QuadBatchCount++;
    }

    void GLRenderer::flushQuadBatch() {
        if (m_QuadBatchCount == 0) return;

        m_SpriteShader.use();
        updateUniforms();
        m_SpriteShader.setInt("u_UseTexture", m_CurrentBatchTexture != 0 ? 1 : 0);

        if (m_CurrentBatchTexture != 0) {
            bindTexture(m_CurrentBatchTexture);
            m_SpriteShader.setInt("u_Texture", 0);
        }

        else bindTexture(0);

        bindVAO(m_QuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);

        size_t vertexBytes = m_QuadBatchVertices.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBytes, m_QuadBatchVertices.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadEBO);

        size_t indexBytes = m_QuadBatchIndices.size() * sizeof(uint32_t);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexBytes, m_QuadBatchIndices.data());

        // Draw all quads in one call
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_QuadBatchIndices.size()), GL_UNSIGNED_INT, 0);

        // Clear batch
        m_QuadBatchVertices.clear();
        m_QuadBatchIndices.clear();

        m_QuadBatchCount = 0;
        m_CurrentBatchTexture = 0;
    }

    void GLRenderer::addLineToBatch(const Vector2& start, const Vector2& end, const Color& color) {
        if (m_LineBatchCount >= MAX_LINES_PER_BATCH) {
            flushLineBatch();
        }

        // Add line vertices (2 vertices per line, 6 floats each: x, y, r, g, b, a)
        float x1 = start.getX();
        float y1 = start.getY();
        float x2 = end.getX();
        float y2 = end.getY();

        // Start vertex
        m_LineBatchVertices.push_back(x1);
        m_LineBatchVertices.push_back(y1);
        m_LineBatchVertices.push_back(color.r);
        m_LineBatchVertices.push_back(color.g);
        m_LineBatchVertices.push_back(color.b);
        m_LineBatchVertices.push_back(color.a);

        // End vertex
        m_LineBatchVertices.push_back(x2);
        m_LineBatchVertices.push_back(y2);
        m_LineBatchVertices.push_back(color.r);
        m_LineBatchVertices.push_back(color.g);
        m_LineBatchVertices.push_back(color.b);
        m_LineBatchVertices.push_back(color.a);

        m_LineBatchCount++;
    }

    void GLRenderer::flushLineBatch() {
        if (m_LineBatchCount == 0) return;

        m_SpriteShader.use();
        updateUniforms();
        m_SpriteShader.setInt("u_UseTexture", 0);
        bindTexture(0);

        bindVAO(m_LineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);

        size_t lineBytes = m_LineBatchVertices.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, 0, lineBytes, m_LineBatchVertices.data());

        // Draw all lines in one call
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_LineBatchCount * 2));

        // Clear batch
        m_LineBatchVertices.clear();
        m_LineBatchCount = 0;
    }
}
