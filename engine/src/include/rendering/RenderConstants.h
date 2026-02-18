#pragma once

#include <utils/math.h>
#include <utils/rendering.h>

using namespace engine::utils::math;
using namespace engine::utils::rendering;

namespace engine::rendering {
    struct Vertex2D {
        Vector2 position;
        Vector2 texCoord;
        Color color;

        Vertex2D(const Vector2& pos, const Vector2& tex, const Color& col) : position(pos), texCoord(tex), color(col) {}
    };

    struct LineVertex {
        Vector2 position;
        Color color;

        LineVertex(const Vector2& pos, const Color& col) : position(pos), color(col) {}
    };

    // Vertex data layout information
    constexpr size_t VERTEX2D_SIZE = sizeof(float) * 8; // 2 pos + 2 tex + 4 color
    constexpr size_t LINE_VERTEX_SIZE = sizeof(float) * 6; // 2 pos + 4 color
    constexpr size_t VERTEX2D_POSITION_OFFSET = 0;
    constexpr size_t VERTEX2D_TEXCOORD_OFFSET = sizeof(float) * 2;
    constexpr size_t VERTEX2D_COLOR_OFFSET = sizeof(float) * 4;

    // Unit quad template (0,0 to 1,1) - will be scaled and positioned dynamically
    inline const std::array<Vertex2D, 4> UNIT_QUAD_VERTICES = {{
        Vertex2D({0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}), // Bottom-left
        Vertex2D({1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}), // Bottom-right
        Vertex2D({1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}), // Top-right
        Vertex2D({0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}), // Top-left
    }};

    // Quad indices forming two triangles (counter-clockwise winding)
    constexpr std::array<uint32_t, 6> QUAD_INDICES = {
        0, 1, 2,  // First triangle
        2, 3, 0   // Second triangle
    };

    // Maximum vertices per batch (for future sprite batching)
    constexpr size_t MAX_SPRITES_PER_BATCH = 10000;
    constexpr size_t MAX_VERTICES_PER_BATCH = MAX_SPRITES_PER_BATCH * 4;
    constexpr size_t MAX_INDICES_PER_BATCH = MAX_SPRITES_PER_BATCH * 6;

    // Helper function to convert structured vertices to flat array for OpenGL
    template<size_t N>
    inline std::array<float, N * 8> verticesToFloatArray(const std::array<Vertex2D, N>& vertices) {
        std::array<float, N * 8> result;
        
        for (size_t i = 0; i < N; ++i) {
            const auto& v = vertices[i];
            size_t offset = i * 8;
            
            result[offset + 0] = v.position.getX();
            result[offset + 1] = v.position.getY();
            result[offset + 2] = v.texCoord.getX();
            result[offset + 3] = v.texCoord.getY();
            result[offset + 4] = v.color.r;
            result[offset + 5] = v.color.g;
            result[offset + 6] = v.color.b;
            result[offset + 7] = v.color.a;
        }
        
        return result;
    }

    // Helper function to convert line vertices to flat array for OpenGL
    template<size_t N>
    inline std::array<float, N * 6> lineVerticesToFloatArray(const std::array<LineVertex, N>& vertices) {
        std::array<float, N * 6> result;
        
        for (size_t i = 0; i < N; ++i) {
            const auto& v = vertices[i];
            size_t offset = i * 6;
            
            result[offset + 0] = v.position.getX();
            result[offset + 1] = v.position.getY();
            result[offset + 2] = v.color.r;
            result[offset + 3] = v.color.g;
            result[offset + 4] = v.color.b;
            result[offset + 5] = v.color.a;
        }
        
        return result;
    }
}
