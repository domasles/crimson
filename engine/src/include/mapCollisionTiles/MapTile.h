#pragma once

#include <mapCollisionTiles/BaseTile.h>

namespace engine::mapCollisionTiles {
    class MapTile : public BaseTile {
        public:
            MapTile() = default;
            virtual ~MapTile() = default;

            void init() override;
            void init(const Vector2& position) override;
            void init(const Vector2& position, const Vector2& size);
            void render() override;

            void setTileSize(const Vector2& size);

        protected:
            void configureCollision() override;

        private:
            Vector2 m_TileSize{32.0f, 32.0f};
    };
}
