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
        addRectToBatch(position.getX(), position.getY(), size.getX(), size.getY(), color);
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

        // Pre-calculate all coordinates once
        float x = position.getX();
        float y = position.getY();
        float w = size.getX();
        float h = size.getY();

        float u0 = texCoordMin.getX();
        float v0 = texCoordMin.getY();
        float u1 = texCoordMax.getX();
        float v1 = texCoordMax.getY();

        // Resize both arrays once for this quad
        size_t vertOffset = m_QuadBatchVertices.size();
        size_t indexOffset = m_QuadBatchIndices.size();

        m_QuadBatchVertices.resize(vertOffset + 32); // 4 vertices × 8 floats each
        m_QuadBatchIndices.resize(indexOffset + 6);  // 6 indices (2 triangles)

        float* vData = m_QuadBatchVertices.data() + vertOffset;
        uint32_t* iData = m_QuadBatchIndices.data() + indexOffset;

        uint32_t baseVertex = static_cast<uint32_t>(m_QuadBatchCount * 4);

        // Vertex 0: bottom-left (x, y, u0, v0, r, g, b, a)
        vData[0] = x;      vData[1] = y;
        vData[2] = u0;     vData[3] = v0;
        vData[4] = tint.r; vData[5] = tint.g; vData[6] = tint.b; vData[7] = tint.a;

        // Vertex 1: bottom-right (x+w, y, u1, v0, r, g, b, a)
        vData[8] = x + w;  vData[9] = y;
        vData[10] = u1;    vData[11] = v0;
        vData[12] = tint.r; vData[13] = tint.g; vData[14] = tint.b; vData[15] = tint.a;

        // Vertex 2: top-right (x+w, y+h, u1, v1, r, g, b, a)
        vData[16] = x + w; vData[17] = y + h;
        vData[18] = u1;    vData[19] = v1;
        vData[20] = tint.r; vData[21] = tint.g; vData[22] = tint.b; vData[23] = tint.a;

        // Vertex 3: top-left (x, y+h, u0, v1, r, g, b, a)
        vData[24] = x;     vData[25] = y + h;
        vData[26] = u0;    vData[27] = v1;
        vData[28] = tint.r; vData[29] = tint.g; vData[30] = tint.b; vData[31] = tint.a;

        // Indices (two triangles: 0-1-2 and 2-3-0)
        iData[0] = baseVertex + 0; iData[1] = baseVertex + 1; iData[2] = baseVertex + 2;
        iData[3] = baseVertex + 2; iData[4] = baseVertex + 3; iData[5] = baseVertex + 0;

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
        
        // Must bind buffers before glBufferSubData
        glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
        size_t vertexBytes = m_QuadBatchVertices.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBytes, m_QuadBatchVertices.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadEBO);
        size_t indexBytes = m_QuadBatchIndices.size() * sizeof(uint32_t);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexBytes, m_QuadBatchIndices.data());

        // Draw all quads in one call
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_QuadBatchIndices.size()), GL_UNSIGNED_INT, 0);

        // Reset batch
        m_QuadBatchVertices.resize(0);
        m_QuadBatchIndices.resize(0);

        m_QuadBatchCount = 0;
        m_CurrentBatchTexture = 0;
    }

    void GLRenderer::addLineToBatch(const Vector2& start, const Vector2& end, const Color& color) {
        if (m_LineBatchCount >= MAX_LINES_PER_BATCH) {
            flushLineBatch();
        }

        size_t offset = m_LineBatchVertices.size();
        m_LineBatchVertices.resize(offset + 12);  // 2 vertices × 6 floats each

        float* data = m_LineBatchVertices.data() + offset;

        // Get coordinates once
        float x1 = start.getX();
        float y1 = start.getY();
        float x2 = end.getX();
        float y2 = end.getY();

        // Start vertex (x, y, r, g, b, a)
        data[0] = x1;
        data[1] = y1;
        data[2] = color.r;
        data[3] = color.g;
        data[4] = color.b;
        data[5] = color.a;

        // End vertex (x, y, r, g, b, a)
        data[6] = x2;
        data[7] = y2;
        data[8] = color.r;
        data[9] = color.g;
        data[10] = color.b;
        data[11] = color.a;

        m_LineBatchCount++;
    }

    void GLRenderer::addRectToBatch(float x, float y, float w, float h, const Color& color) {
        if (m_LineBatchCount + 4 > MAX_LINES_PER_BATCH) {
            flushLineBatch();
        }

        size_t offset = m_LineBatchVertices.size();
        m_LineBatchVertices.resize(offset + 48);  // 4 lines × 2 vertices × 6 floats

        float* data = m_LineBatchVertices.data() + offset;

        // Bottom line (x, y) → (x+w, y)
        data[0] = x;       data[1] = y;
        data[2] = color.r; data[3] = color.g; data[4] = color.b; data[5] = color.a;
        data[6] = x + w;   data[7] = y;
        data[8] = color.r; data[9] = color.g; data[10] = color.b; data[11] = color.a;

        // Right line (x+w, y) → (x+w, y+h)
        data[12] = x + w;  data[13] = y;
        data[14] = color.r; data[15] = color.g; data[16] = color.b; data[17] = color.a;
        data[18] = x + w;  data[19] = y + h;
        data[20] = color.r; data[21] = color.g; data[22] = color.b; data[23] = color.a;

        // Top line (x+w, y+h) → (x, y+h)
        data[24] = x + w;  data[25] = y + h;
        data[26] = color.r; data[27] = color.g; data[28] = color.b; data[29] = color.a;
        data[30] = x;      data[31] = y + h;
        data[32] = color.r; data[33] = color.g; data[34] = color.b; data[35] = color.a;

        // Left line (x, y+h) → (x, y)
        data[36] = x;      data[37] = y + h;
        data[38] = color.r; data[39] = color.g; data[40] = color.b; data[41] = color.a;
        data[42] = x;      data[43] = y;
        data[44] = color.r; data[45] = color.g; data[46] = color.b; data[47] = color.a;

        m_LineBatchCount += 4;
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

        // Reset batch
        m_LineBatchVertices.resize(0);
        m_LineBatchCount = 0;
    }
}
