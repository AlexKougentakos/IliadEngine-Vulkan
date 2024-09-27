#include "SceneManager.h"

namespace ili 
{
    Scene* SceneManager::CreateScene(const std::string& sceneName)
    {
		const bool makeCurrentScene = m_Scenes.empty();

        // Check if a scene with the same name already exists
        if (m_Scenes.contains(sceneName)) return nullptr;

        m_Scenes[sceneName] = std::unique_ptr<Scene>(new Scene());

		// If this is the first scene, set it as the current scene
		if (makeCurrentScene) m_pCurrentScene = m_Scenes[sceneName].get();

        return m_Scenes[sceneName].get();
    }

    Scene* SceneManager::GetScene(const std::string& sceneName)
    {
        if (!m_Scenes.contains(sceneName)) return nullptr;

        return m_Scenes[sceneName].get();
    }

    void SceneManager::RemoveScene(const std::string& sceneName) 
    {
        m_Scenes.erase(sceneName);
    }
}