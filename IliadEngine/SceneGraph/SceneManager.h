#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include "Scene.h"

namespace ili {

    class SceneManager final
    {
    public:
		SceneManager() = default;
		~SceneManager() = default;

		SceneManager(const SceneManager&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;

		SceneManager(SceneManager&&) = delete;
		SceneManager& operator=(SceneManager&&) = delete;

        // Templated function definition
        template<typename SceneType, typename... Args>
        SceneType* CreateScene(const std::string& sceneName, Args&&... args)
        {
            static_assert(std::is_base_of<Scene, SceneType>::value, "SceneType must derive from Scene");

            const bool makeCurrentScene = m_Scenes.empty();

            // Check if a scene with the same name already exists
            if (m_Scenes.contains(sceneName)) return nullptr; // Scene with this name already exists

            // Create a new SceneType instance and store it in m_Scenes
            auto scene = std::make_unique<SceneType>(std::forward<Args>(args)...);
			scene->Initialize();
            SceneType* scenePtr = scene.get();
            m_Scenes[sceneName] = std::move(scene);

            // If this is the first scene, set it as the current scene
            if (makeCurrentScene) m_pCurrentScene = scenePtr;

            return scenePtr;
        }

        Scene* GetScene(const std::string& sceneName);
		Scene* GetCurrentScene() const { return m_pCurrentScene; }

        void RemoveScene(const std::string& sceneName);

        void Update(const float deltaTime);

    private:
        std::unordered_map<std::string, std::unique_ptr<Scene>> m_Scenes;
		Scene* m_pCurrentScene{ nullptr };
    };
}
