#include "ModelComponent.h"

#include "Core/ContentLoader.h"

namespace ili
{
	ModelComponent::ModelComponent(const std::shared_ptr<Model>& pModel) : m_pModel(pModel)
	{
		m_pMaterial = std::make_shared<Material>();
	}

	ModelComponent::ModelComponent(const std::string& modelPath)
	{
		m_pMaterial = std::make_shared<Material>();
		m_pModel = ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/" + modelPath + ".obj");
	}

	void ModelComponent::Initialize()
	{

	}
}