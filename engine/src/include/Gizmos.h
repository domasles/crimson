#pragma once

namespace engine {
    enum class GizmoType {
        COLLISION_BOUNDS,
        TEXTURE_BOUNDS
    };

    class Gizmos {
        public:
            static void setChannel(GizmoType type, bool enabled);
            static bool isChannelEnabled(GizmoType type);

            static void enableAll();
            static void disableAll();
            
            static void renderGizmos();

        private:
            static bool s_CollisionBoundsEnabled;
            static bool s_TextureBoundsEnabled;
    };
}
