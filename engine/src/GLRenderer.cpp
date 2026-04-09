#include <pch.h>

#include <utils/logger.h>
#include <utils/rendering.h>

#include <rendering/RenderConstants.h>

#include <GLRenderer.h>
#include <Core.h>

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

        #ifndef ENGINE_PLATFORM_EMSCRIPTEN
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
        ENGINE_LOG_INIT("GL Renderer");

        return true;
    }

    void GLRenderer::shutdown() {
        if (!m_Initialized) {
            return;
        }

        destroyBuffers();
        m_Initialized = false;
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
            case RenderPass::UI:
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
        glEnable(GL_SCISSOR_TEST);
        glScissor(x, y, width, height);
    }

    void GLRenderer::drawQuad(const Vector2& position, const Vector2& size, GLuint textureID, const Color& tint, float rotation, const Vector2& scale) {
        drawQuad(position, size, {0.0f, 0.0f}, {1.0f, 1.0f}, textureID, tint, rotation, scale);
    }

    void GLRenderer::drawQuad(const Vector2& position, const Vector2& size, const Vector2& texCoordMin, const Vector2& texCoordMax, GLuint textureID, const Color tint, float rotation, const Vector2& scale) {
        addQuadToBatch(position, size, texCoordMin, texCoordMax, textureID, tint, rotation, scale);
    }

    void GLRenderer::drawRect(const Vector2& position, const Vector2& size, const Color& color, float rotation) {
        addRectToBatch(position.getX(), position.getY(), size.getX(), size.getY(), color, rotation);
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

    void GLRenderer::applyCameraProjection(const Vector2& camPos, float zoom, float viewW, float viewH) {
        float halfW = viewW / (2.0f * zoom);
        float halfH = viewH / (2.0f * zoom);

        setOrthographicProjection(
            camPos.getRawX() - halfW,
            camPos.getRawX() + halfW,
            camPos.getRawY() + halfH,
            camPos.getRawY() - halfH
        );
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

    uint32_t GLRenderer::createGeometry(const float* vertexData, size_t vertexCount, const int* indices, size_t indexCount) {
        if (!vertexData || vertexCount == 0 || !indices || indexCount == 0) return 0;

        GeometryData data;

        glGenVertexArrays(1, &data.vao);
        glGenBuffers(1, &data.vbo);
        glGenBuffers(1, &data.ebo);

        glBindVertexArray(data.vao);

        glBindBuffer(GL_ARRAY_BUFFER, data.vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexCount * sizeof(float) * 8), vertexData, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indexCount * sizeof(int)), indices, GL_STATIC_DRAW);

        constexpr GLsizei stride = 8 * sizeof(float);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(4 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        data.indexCount = static_cast<int>(indexCount);
        uint32_t handle = m_NextGeometryHandle++;
        m_GeometryData[handle] = data;

        return handle;
    }

    void GLRenderer::destroyGeometry(uint32_t geometry) {
        auto it = m_GeometryData.find(geometry);
        if (it == m_GeometryData.end()) return;

        GeometryData& data = it->second;

        if (data.vao) glDeleteVertexArrays(1, &data.vao);
        if (data.vbo) glDeleteBuffers(1, &data.vbo);
        if (data.ebo) glDeleteBuffers(1, &data.ebo);

        m_GeometryData.erase(it);
    }

    void GLRenderer::drawGeometry(uint32_t geometry, GLuint texture, const Vector2& translation) {
        auto it = m_GeometryData.find(geometry);
        if (it == m_GeometryData.end()) return;

        GeometryData& data = it->second;
        m_SpriteShader.use();
        updateUniforms();

        m_SpriteShader.setInt("u_UseTexture", texture != 0 ? 1 : 0);
        m_SpriteShader.setInt("u_Texture", 0);
        m_SpriteShader.setVec2("u_Translation", translation);

        bindTexture(texture);
        bindVAO(data.vao);
        glDrawElements(GL_TRIANGLES, data.indexCount, GL_UNSIGNED_INT, nullptr);
    }

    GLuint GLRenderer::createTexture(int width, int height, const void* pixels) {
        if (width <= 0 || height <= 0) return 0;
        GLuint id = 0;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);

        return id;
    }

    void GLRenderer::destroyTexture(GLuint texture) {
        if (texture != 0) {
            glDeleteTextures(1, &texture);
        }
    }

    void GLRenderer::enableScissor(bool enable) {
        if (enable) glEnable(GL_SCISSOR_TEST);
        else glDisable(GL_SCISSOR_TEST);
    }

    void GLRenderer::setScissorRegion(int x, int y, int width, int height) {
        glScissor(x, y, width, height);
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

    void GLRenderer::addQuadToBatch(const Vector2& position, const Vector2& size, const Vector2& texCoordMin, const Vector2& texCoordMax, GLuint textureID, const Color& tint, float rotation, const Vector2& scale) {
        if ((m_CurrentBatchTexture != textureID && m_QuadBatchCount > 0) || m_QuadBatchCount >= MAX_QUADS_PER_BATCH) {
            flushQuadBatch();
        }

        m_CurrentBatchTexture = textureID;

        // Pre-calculate all coordinates
        float x = position.getX();
        float y = position.getY();
        float w = size.getX() * scale.getX();
        float h = size.getY() * scale.getY();

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

        // Define quad corners
        float corners[4][2] = {
            {0.0f, 0.0f}, // bottom-left
            {w, 0.0f},    // bottom-right
            {w, h},       // top-right
            {0.0f, h}     // top-left
        };

        // Apply rotation if needed
        if (rotation != 0.0f) {
            float rad = rotation * 3.14159265f / 180.0f;

            float cosR = std::cos(rad);
            float sinR = std::sin(rad);

            float centerX = w / 2.0f;
            float centerY = h / 2.0f;

            for (int i = 0; i < 4; i++) {
                float relX = corners[i][0] - centerX;
                float relY = corners[i][1] - centerY;

                corners[i][0] = centerX + (relX * cosR - relY * sinR);
                corners[i][1] = centerY + (relX * sinR + relY * cosR);
            }
        }

        // Vertex 0: bottom-left
        vData[0] = x + corners[0][0];  vData[1] = y + corners[0][1];
        vData[2] = u0;                  vData[3] = v0;
        vData[4] = tint.r; vData[5] = tint.g; vData[6] = tint.b; vData[7] = tint.a;

        // Vertex 1: bottom-right
        vData[8] = x + corners[1][0];   vData[9] = y + corners[1][1];
        vData[10] = u1;                 vData[11] = v0;
        vData[12] = tint.r; vData[13] = tint.g; vData[14] = tint.b; vData[15] = tint.a;

        // Vertex 2: top-right
        vData[16] = x + corners[2][0];  vData[17] = y + corners[2][1];
        vData[18] = u1;                 vData[19] = v1;
        vData[20] = tint.r; vData[21] = tint.g; vData[22] = tint.b; vData[23] = tint.a;

        // Vertex 3: top-left
        vData[24] = x + corners[3][0];  vData[25] = y + corners[3][1];
        vData[26] = u0;                 vData[27] = v1;
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

    void GLRenderer::addRectToBatch(float x, float y, float w, float h, const Color& color, float rotation) {
        if (m_LineBatchCount + 4 > MAX_LINES_PER_BATCH) {
            flushLineBatch();
        }

        size_t offset = m_LineBatchVertices.size();
        m_LineBatchVertices.resize(offset + 48);  // 4 lines × 2 vertices × 6 floats

        float* data = m_LineBatchVertices.data() + offset;

        // Define rect corners (before rotation)
        float corners[4][2] = {
            {0.0f, 0.0f},  // bottom-left
            {w, 0.0f},     // bottom-right
            {w, h},        // top-right
            {0.0f, h}      // top-left
        };

        // Apply rotation if needed
        if (rotation != 0.0f) {
            float rad = rotation * 3.14159265f / 180.0f;
            float cosR = std::cos(rad);
            float sinR = std::sin(rad);
            float centerX = w / 2.0f;
            float centerY = h / 2.0f;

            for (int i = 0; i < 4; i++) {
                float relX = corners[i][0] - centerX;
                float relY = corners[i][1] - centerY;
                corners[i][0] = centerX + (relX * cosR - relY * sinR);
                corners[i][1] = centerY + (relX * sinR + relY * cosR);
            }
        }

        // Bottom line: corner 0 → corner 1
        data[0] = x + corners[0][0];  data[1] = y + corners[0][1];
        data[2] = color.r; data[3] = color.g; data[4] = color.b; data[5] = color.a;
        data[6] = x + corners[1][0];  data[7] = y + corners[1][1];
        data[8] = color.r; data[9] = color.g; data[10] = color.b; data[11] = color.a;

        // Right line: corner 1 → corner 2
        data[12] = x + corners[1][0]; data[13] = y + corners[1][1];
        data[14] = color.r; data[15] = color.g; data[16] = color.b; data[17] = color.a;
        data[18] = x + corners[2][0]; data[19] = y + corners[2][1];
        data[20] = color.r; data[21] = color.g; data[22] = color.b; data[23] = color.a;

        // Top line: corner 2 → corner 3
        data[24] = x + corners[2][0]; data[25] = y + corners[2][1];
        data[26] = color.r; data[27] = color.g; data[28] = color.b; data[29] = color.a;
        data[30] = x + corners[3][0]; data[31] = y + corners[3][1];
        data[32] = color.r; data[33] = color.g; data[34] = color.b; data[35] = color.a;

        // Left line: corner 3 → corner 0
        data[36] = x + corners[3][0]; data[37] = y + corners[3][1];
        data[38] = color.r; data[39] = color.g; data[40] = color.b; data[41] = color.a;
        data[42] = x + corners[0][0]; data[43] = y + corners[0][1];
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
