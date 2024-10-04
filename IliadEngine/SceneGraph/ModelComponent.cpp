#include "ModelComponent.h"

#include "Core/ContentLoader.h"

namespace ili
{

	ModelComponent::ModelComponent(const std::string& modelPath)
	{
		m_pModel = ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/" + modelPath + ".obj");
	}

	void ModelComponent::Initialize()
	{

	}
}