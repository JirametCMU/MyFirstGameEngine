#pragma once
#include <string>

namespace Game {
    /** Enum listing all available high-level game scenes. */
    enum class SceneId {
        Menu,
        Gameplay
    };

    /** Utility to convert SceneId enums to string identifiers used by SceneManager. */
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
