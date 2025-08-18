#pragma once

namespace engine {
    class Entity;
    
    class Component {
        public:
            Component() = default;
            virtual ~Component() = default;

            virtual void init() {}
            virtual void update(float deltaTime) {}
            virtual void onDestroy() {}

            void setEntity(Entity* entity) { m_Entity = entity; }
            Entity* getEntity() const { return m_Entity; }

        protected:
            Entity* m_Entity = nullptr;
    };
}
