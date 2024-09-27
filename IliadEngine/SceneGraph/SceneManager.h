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

        Scene* CreateScene(const std::string& sceneName);

        Scene* GetScene(const std::string& sceneName);
		Scene* GetCurrentScene() const { return m_pCurrentScene; }

        void RemoveScene(const std::string& sceneName);

        void Update(const float deltaTime);

    private:
        std::unordered_map<std::string, std::unique_ptr<Scene>> m_Scenes;
		Scene* m_pCurrentScene{ nullptr };
    };
}
