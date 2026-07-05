#include "Engine/SceneManager.hpp"
#include "Engine/Log.hpp"
#include <stdexcept>

namespace Engine {

    SceneManager::~SceneManager()
    {
        // Exit the active scene before destroying everything
        if (m_ActiveScene != nullptr)
        {
            m_ActiveScene->OnExit();
            m_ActiveScene = nullptr;
        }
    }

    void SceneManager::AddScene(const std::string& name, std::unique_ptr<Scene> scene)
    {
        if (m_Scenes.contains(name))
        {
            ENGINE_WARN("[SceneManager] Warning: Overwriting existing scene \"{}\"", name);
        }

        ENGINE_INFO("[SceneManager] Registered scene: \"{}\"", name);
        m_Scenes[name] = std::move(scene);
    }

    void SceneManager::SwitchTo(const std::string& name)
    {
        auto it = m_Scenes.find(name);
        if (it == m_Scenes.end())
        {
            ENGINE_ERROR("[SceneManager] Error: Scene \"{}\" not found!", name);
            return;
        }

        // Exit the current scene
        if (m_ActiveScene != nullptr)
        {
            ENGINE_INFO("[SceneManager] Exiting scene: \"{}\"", m_CurrentName);
            m_ActiveScene->OnExit();
        }

        // Enter the new scene
        m_ActiveScene = it->second.get();
        m_CurrentName = name;
        ENGINE_INFO("[SceneManager] Entering scene: \"{}\"", m_CurrentName);
        m_ActiveScene->OnEnter();
    }

    void SceneManager::Update(float deltaTime)
    {
        if (m_ActiveScene)
        {
            m_ActiveScene->OnUpdate(deltaTime);
        }
    }

    void SceneManager::FixedUpdate(float fixedDeltaTime)
    {
        if (m_ActiveScene)
        {
            m_ActiveScene->OnFixedUpdate(fixedDeltaTime);
        }
    }

    void SceneManager::Render(sf::RenderWindow& window, float alpha)
    {
        if (m_ActiveScene != nullptr)
        {
            m_ActiveScene->OnRender(window, alpha);
        }
    }

} // namespace Engine
