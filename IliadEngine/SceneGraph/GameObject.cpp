#include "GameObject.h"

namespace ili
{
	GameObject::GameObject(const unsigned id) : m_Id(id)
	{
		m_pTransformComponent = AddComponent<TransformComponent>();
	}

	void GameObject::RootUpdate()
	{
		Update();
	}

	//GameObject GameObject::MakePointLight(float intensity, float radius, glm::vec3 color)
	//{
	//	/*GameObject pointLight = GameObject::Create();
	//	pointLight.SetColor(color);
	//	pointLight.GetTransform().scale.x = radius;
	//	pointLight.m_PointLightComponent = std::make_unique<PointLightComponent>();
	//	pointLight.m_PointLightComponent->lightIntensity = intensity;
	//	return pointLight;*/
	//	return{0};
	//}
}
