#pragma once

#include <GLES3/gl3.h>

#include <Texture.h>

namespace engine {
    class GLRenderer;
}

namespace engine::ui {
    struct UIGeometryData {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;

        int indexCount = 0;
    };

    class UIRenderBackend : public Rml::RenderInterface {
        public:
            UIRenderBackend() = default;
            ~UIRenderBackend() override { shutdown(); }

            bool init(engine::GLRenderer* renderer, int screenWidth, int screenHeight);
            void shutdown();

            void beginUIPass();
            void endUIPass();

            Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;

            void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) override;
            void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

            Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
            Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;

            void ReleaseTexture(Rml::TextureHandle texture) override;

            void EnableScissorRegion(bool enable) override;
            void SetScissorRegion(Rml::Rectanglei region) override;

        private:
            engine::GLRenderer* m_Renderer = nullptr;

            std::array<float, 16> m_SavedProjection{};

            std::unordered_map<Rml::CompiledGeometryHandle, UIGeometryData> m_Geometries;
            Rml::CompiledGeometryHandle m_NextHandle = 1;

            std::unordered_map<Rml::TextureHandle, std::shared_ptr<engine::Texture>> m_ManagedTextures;
            std::unordered_set<GLuint> m_GeneratedTextures;
    };
}
