#include "Scene.h"

namespace ili 
{
    GameObject* Scene::CreateGameObject()
    {
		m_pGameObjects.emplace_back(std::unique_ptr<GameObject>(new GameObject(m_IdCounter++)));
		return m_pGameObjects.back().get();
    }

    void Scene::RemoveGameObject(unsigned int id) 
	{
		const auto it = std::ranges::find_if(m_pGameObjects, [id](const std::unique_ptr<GameObject>& pGameObject) { return pGameObject->GetId() == id; });

		if (it != m_pGameObjects.end())
		{
			m_pGameObjects.erase(it);
		}
    }

    std::vector<std::unique_ptr<GameObject>>& Scene::GetGameObjects()
    {
        return m_pGameObjects;
    }

    void Scene::Update(float deltaTime) 
    {
        // Update game objects, perform scene logic, etc.
    }
}
