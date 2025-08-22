#pragma once

#include <utils/filesystem.h>
#include <utils/math.h>

#include <components/CollisionComponent.h>
#include <components/TransformComponent.h>
#include <components/TextureComponent.h>
#include <components/InputComponent.h>

#include <Component.h>
#include <Resources.h>
#include <Transform.h>
#include <Texture.h>
#include <Entity.h>
#include <Sound.h>

#include <Scene.h>
#include <Map.h>

#include <Input.h>
#include <Core.h>

using namespace engine;

namespace engine::utils {
    using namespace math;
    using namespace logger;
    using namespace collision;
    using namespace filesystem;
}

namespace engine::collisions {
    using namespace types;
    using namespace shapes;
}
