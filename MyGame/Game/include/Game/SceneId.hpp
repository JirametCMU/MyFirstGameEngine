#pragma once
#include <string>

namespace Game {
    enum class SceneId {
        Menu,
        Gameplay
    };

    class SceneRegistry {
    public:
        static std::string Name(SceneId id) {
            switch (id) {
                case SceneId::Menu: return "menu";
                case SceneId::Gameplay: return "gameplay";
                default: return "unknown";
            }
        }
    };
}
