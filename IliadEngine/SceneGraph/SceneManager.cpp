#include "SceneManager.h"

namespace ili 
{
    Scene* SceneManager::GetScene(const std::string& sceneName)
    {
        if (!m_Scenes.contains(sceneName)) return nullptr;

        return m_Scenes[sceneName].get();
    }

    void SceneManager::RemoveScene(const std::string& sceneName) 
    {
        m_Scenes.erase(sceneName);
    }

    void SceneManager::Update(const float deltaTime)
    {
		if (m_pCurrentScene)
		{
			m_pCurrentScene->Update(deltaTime);
		}
    }
}