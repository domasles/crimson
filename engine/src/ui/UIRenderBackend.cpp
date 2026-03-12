#include <pch.h>

#include <ui/UIRenderBackend.h>
#include <utils/logger.h>

#include <GLRenderer.h>
#include <Resources.h>
#include <Shader.h>

using namespace engine::utils::logger;
using namespace engine::utils::math;

namespace engine::ui {
    bool UIRenderBackend::init(engine::GLRenderer* renderer, int screenWidth, int screenHeight) {
        m_Renderer = renderer;
        m_PhysWidth = screenWidth;
        m_PhysHeight = screenHeight;

        ENGINE_LOG_INIT("UI Render Backend");

        return true;
    }

    void UIRenderBackend::shutdown() {
        for (auto& [handle, data] : m_Geometries) {
            glDeleteVertexArrays(1, &data.vao);
            glDeleteBuffers(1, &data.vbo);
            glDeleteBuffers(1, &data.ebo);
        }

        m_Geometries.clear();

        for (GLuint id : m_GeneratedTextures) {
            glDeleteTextures(1, &id);
        }

        m_GeneratedTextures.clear();
        m_ManagedTextures.clear();
    }

    void UIRenderBackend::beginUIPass() {
        std::memcpy(m_SavedProjection.data(), m_Renderer->getProjectionMatrix(), 16 * sizeof(float));
        glGetIntegerv(GL_VIEWPORT, m_SavedViewport.data());

        glViewport(0, 0, m_PhysWidth, m_PhysHeight);
        m_Renderer->setOrthographicProjection(0.0f, static_cast<float>(m_PhysWidth), static_cast<float>(m_PhysHeight), 0.0f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
    }

    void UIRenderBackend::endUIPass() {
        Shader* shader = m_Renderer->getDefaultShader();

        if (shader) {
            shader->use();
            shader->setVec2("u_Translation", Vector2{0.0f, 0.0f});
        }

        glDisable(GL_SCISSOR_TEST);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        m_Renderer->setProjectionMatrix(m_SavedProjection.data());
        glViewport(m_SavedViewport[0], m_SavedViewport[1], m_SavedViewport[2], m_SavedViewport[3]);
    }

    Rml::CompiledGeometryHandle UIRenderBackend::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) {
        std::vector<float> vboData;
        vboData.reserve(vertices.size() * 8);

        for (const auto& v : vertices) {
            vboData.push_back(v.position.x);
            vboData.push_back(v.position.y);
            vboData.push_back(v.tex_coord.x);
            vboData.push_back(v.tex_coord.y);
            vboData.push_back(v.colour.red   / 255.0f);
            vboData.push_back(v.colour.green / 255.0f);
            vboData.push_back(v.colour.blue  / 255.0f);
            vboData.push_back(v.colour.alpha / 255.0f);
        }

        UIGeometryData data;
        data.indexCount = static_cast<int>(indices.size());

        glGenVertexArrays(1, &data.vao);
        glGenBuffers(1, &data.vbo);
        glGenBuffers(1, &data.ebo);

        glBindVertexArray(data.vao);

        glBindBuffer(GL_ARRAY_BUFFER, data.vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vboData.size() * sizeof(float)), vboData.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(int)), indices.data(), GL_STATIC_DRAW);

        constexpr GLsizei stride = 8 * sizeof(float);

        // layout 0: position (vec2)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(0));

        // layout 1: tex coord (vec2)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(2 * sizeof(float)));

        // layout 2: color (vec4)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(4 * sizeof(float)));

        glBindVertexArray(0);

        Rml::CompiledGeometryHandle handle = m_NextHandle++;
        m_Geometries[handle] = data;

        return handle;
    }

    void UIRenderBackend::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) {
        auto it = m_Geometries.find(geometry);
        if (it == m_Geometries.end()) return;

        const UIGeometryData& data = it->second;
        Shader* shader = m_Renderer->getDefaultShader();

        shader->use();
        shader->setMat4("u_Projection", m_Renderer->getProjectionMatrix());
        shader->setVec2("u_Translation", Vector2{translation.x, translation.y});

        if (texture != 0) {
            glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(texture));
            shader->setInt("u_UseTexture", 1);
        }

        else {
            glBindTexture(GL_TEXTURE_2D, 0);
            shader->setInt("u_UseTexture", 0);
        }

        glBindVertexArray(data.vao);
        glDrawElements(GL_TRIANGLES, data.indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void UIRenderBackend::ReleaseGeometry(Rml::CompiledGeometryHandle geometry) {
        auto it = m_Geometries.find(geometry);
        if (it == m_Geometries.end()) return;

        UIGeometryData& data = it->second;

        glDeleteVertexArrays(1, &data.vao);
        glDeleteBuffers(1, &data.vbo);
        glDeleteBuffers(1, &data.ebo);

        m_Geometries.erase(it);
    }

    Rml::TextureHandle UIRenderBackend::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) {
        auto tex = engine::getResources().loadTexture(source);

        if (!tex) {
            Logger::engine_warn("Failed to load texture: {}", source);
            return 0;
        }

        texture_dimensions.x = tex->getWidth();
        texture_dimensions.y = tex->getHeight();

        auto handle = static_cast<Rml::TextureHandle>(tex->getTextureID());
        m_ManagedTextures[handle] = tex;

        return handle;
    }

    Rml::TextureHandle UIRenderBackend::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) {
        GLuint id = 0;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);

        m_GeneratedTextures.insert(id);
        return static_cast<Rml::TextureHandle>(id);
    }

    void UIRenderBackend::ReleaseTexture(Rml::TextureHandle texture) {
        auto rawId = static_cast<GLuint>(texture);
        auto genIt = m_GeneratedTextures.find(rawId);

        if (genIt != m_GeneratedTextures.end()) {
            glDeleteTextures(1, &rawId);
            m_GeneratedTextures.erase(genIt);
            return;
        }

        auto mIt = m_ManagedTextures.find(texture);

        if (mIt != m_ManagedTextures.end()) {
            m_ManagedTextures.erase(mIt);
        }
    }

    void UIRenderBackend::EnableScissorRegion(bool enable) {
        if (enable) glEnable(GL_SCISSOR_TEST);
        else glDisable(GL_SCISSOR_TEST);
    }

    void UIRenderBackend::SetScissorRegion(Rml::Rectanglei region) {
        const int screenY = m_PhysHeight - region.Top() - region.Height();
        glScissor(region.Left(), screenY, region.Width(), region.Height());
    }
}
