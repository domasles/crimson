#include <pch.h>

#include <ui/UIRenderer.h>

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

        ENGINE_LOG_INIT("UI Renderer");

        return true;
    }

    void UIRenderBackend::shutdown() {
        for (auto handle : m_Geometries) {
            m_Renderer->destroyGeometry(handle);
        }

        m_Geometries.clear();

        for (GLuint id : m_GeneratedTextures) {
            m_Renderer->destroyTexture(id);
        }

        m_GeneratedTextures.clear();
        m_ManagedTextures.clear();

        Logger::engine_info("UI Renderer shut down");
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
        std::vector<float> vertexData;
        vertexData.reserve(vertices.size() * 8);

        for (const auto& v : vertices) {
            vertexData.push_back(v.position.x);
            vertexData.push_back(v.position.y);
            vertexData.push_back(v.tex_coord.x);
            vertexData.push_back(v.tex_coord.y);
            vertexData.push_back(v.colour.red / 255.0f);
            vertexData.push_back(v.colour.green / 255.0f);
            vertexData.push_back(v.colour.blue / 255.0f);
            vertexData.push_back(v.colour.alpha / 255.0f);
        }

        Rml::CompiledGeometryHandle handle = m_Renderer->createGeometry(vertexData.data(), vertices.size(), indices.data(), indices.size());

        if (handle != 0) {
            m_Geometries.insert(handle);
        }

        return handle;
    }

    void UIRenderBackend::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) {
        m_Renderer->drawGeometry(geometry, static_cast<GLuint>(texture), Vector2{translation.x, translation.y});
    }

    void UIRenderBackend::ReleaseGeometry(Rml::CompiledGeometryHandle geometry) {
        m_Renderer->destroyGeometry(geometry);
        m_Geometries.erase(geometry);
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
        auto id = m_Renderer->createTexture(source_dimensions.x, source_dimensions.y, source.data());
        if (id == 0) return 0;

        m_GeneratedTextures.insert(id);
        return static_cast<Rml::TextureHandle>(id);
    }

    void UIRenderBackend::ReleaseTexture(Rml::TextureHandle texture) {
        auto rawId = static_cast<GLuint>(texture);
        auto genIt = m_GeneratedTextures.find(rawId);

        if (genIt != m_GeneratedTextures.end()) {
            m_Renderer->destroyTexture(rawId);
            m_GeneratedTextures.erase(genIt);

            return;
        }

        auto mIt = m_ManagedTextures.find(texture);

        if (mIt != m_ManagedTextures.end()) {
            m_ManagedTextures.erase(mIt);
        }
    }

    void UIRenderBackend::EnableScissorRegion(bool enable) {
        m_Renderer->enableScissor(enable);
    }

    void UIRenderBackend::SetScissorRegion(Rml::Rectanglei region) {
        const int screenY = m_PhysHeight - region.Top() - region.Height();
        m_Renderer->setScissorRegion(region.Left(), screenY, region.Width(), region.Height());
    }
}
