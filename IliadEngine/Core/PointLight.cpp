#include "PointLight.h"

ili::PointLightGameObject::PointLightGameObject(unsigned id, float intensity, float radius, glm::vec3 color): 
	GameObject(id),
	m_Intensity(intensity), 
	m_Radius(radius), 
	m_Color(color)
{}