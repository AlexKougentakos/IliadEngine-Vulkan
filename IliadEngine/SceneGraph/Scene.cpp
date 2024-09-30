#include "Scene.h"

namespace ili 
{
	PointLightGameObject* Scene::CreatePointLight(float intensity, float radius, glm::vec3 color)
	{
		const auto light = new PointLightGameObject(m_IdCounter++, intensity, radius, color);
		m_pPointLights.push_back(std::unique_ptr<PointLightGameObject>(light));
		return light;
	}

    void Scene::RemoveGameObject(unsigned int id) 
	{
		const auto it = std::ranges::find_if(m_pGameObjects, [id](const std::unique_ptr<GameObject>& pGameObject) { return pGameObject->GetId() == id; });

		if (it != m_pGameObjects.end())
		{
			m_pGameObjects.erase(it);
		}
    }
}
