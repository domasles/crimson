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
        createIdentityMatrix(m_ViewMatrix.data());

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

    void GLRenderer::clear(const Color& color) {
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
        m_SpriteShader.use();
        updateUniforms();
        m_SpriteShader.setInt("u_UseTexture", textureID != 0 ? 1 : 0);

        if (textureID != 0) {
            bindTexture(textureID);
            m_SpriteShader.setInt("u_Texture", 0);
        } else {
            bindTexture(0);
        }

        // Build quad vertices using structured data
        float x = position.getX();
        float y = position.getY();
        float w = size.getX();
        float h = size.getY();

        std::array<Vertex2D, 4> quadVertices = {{
            Vertex2D({x,     y    }, {texCoordMin.getX(), texCoordMin.getY()}, tint), // Bottom-left
            Vertex2D({x + w, y    }, {texCoordMax.getX(), texCoordMin.getY()}, tint), // Bottom-right
            Vertex2D({x + w, y + h}, {texCoordMax.getX(), texCoordMax.getY()}, tint), // Top-right
            Vertex2D({x,     y + h}, {texCoordMin.getX(), texCoordMax.getY()}, tint), // Top-left
        }};

        // Convert to flat array for OpenGL
        auto vertices = verticesToFloatArray(quadVertices);

        bindVAO(m_QuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void GLRenderer::drawRect(const Vector2& position, const Vector2& size, const Color& color) {
        float x = position.getX();
        float y = position.getY();
        float w = size.getX();
        float h = size.getY();

        // Create structured vertices for rectangle outline
        std::array<LineVertex, 8> vertices = {{
            // Bottom edge
            LineVertex({x,     y    }, color),
            LineVertex({x + w, y    }, color),
            // Right edge
            LineVertex({x + w, y    }, color),
            LineVertex({x + w, y + h}, color),
            // Top edge
            LineVertex({x + w, y + h}, color),
            LineVertex({x,     y + h}, color),
            // Left edge
            LineVertex({x,     y + h}, color),
            LineVertex({x,     y    }, color),
        }};

        auto flatVertices = lineVerticesToFloatArray(vertices);

        m_SpriteShader.use();
        updateUniforms();
        m_SpriteShader.setInt("u_UseTexture", 0);
        bindTexture(0);

        bindVAO(m_LineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, flatVertices.size() * sizeof(float), flatVertices.data());

        glDrawArrays(GL_LINES, 0, 8);
    }

    void GLRenderer::drawLine(const Vector2& start, const Vector2& end, const Color& color) {
        std::array<LineVertex, 2> vertices = {{
            LineVertex(start, color),
            LineVertex(end, color),
        }};

        auto flatVertices = lineVerticesToFloatArray(vertices);

        m_SpriteShader.use();
        updateUniforms();
        m_SpriteShader.setInt("u_UseTexture", 0);
        bindTexture(0);

        bindVAO(m_LineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, flatVertices.size() * sizeof(float), flatVertices.data());

        glDrawArrays(GL_LINES, 0, 2);
    }

    void GLRenderer::setProjectionMatrix(const float* matrix) {
        std::copy(matrix, matrix + 16, m_ProjectionMatrix.begin());
        m_ProjectionDirty = true;
    }

    void GLRenderer::setOrthographicProjection(float left, float right, float bottom, float top) {
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
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, QUAD_INDICES.size() * sizeof(uint32_t), QUAD_INDICES.data(), GL_STATIC_DRAW);

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

        // Reserve space for line vertices (position + color)
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 8, nullptr, GL_DYNAMIC_DRAW);

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
        
        if (m_ViewDirty) {
            m_SpriteShader.setMat4("u_View", m_ViewMatrix.data());
            m_ViewDirty = false;
        }
    }
}
