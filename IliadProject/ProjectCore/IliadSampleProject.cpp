#include "IliadSampleProject .h"

void IliadSampleProject::OnGamePreparing()
{

}

void LoadModels(ili::Device& m_Device, ili::Scene* m_pCurrentScene)
{
	const std::shared_ptr<ili::Model> bunnyModel = ili::Model::CreateModelFromFile(m_Device, "Assets/Models/bunny.obj");

	auto bunny1 = m_pCurrentScene->CreateGameObject();
	bunny1->SetModel(bunnyModel);
	bunny1->GetTransform().position = { -0.5f, 0.f, 0.f };
	bunny1->GetTransform().rotation = { 0.f, 180.f, 0.f };
	bunny1->GetTransform().scale = { 0.5f, -0.5f, 0.5f };


	// Second bunny
	auto bunny2 = m_pCurrentScene->CreateGameObject();
	bunny2->SetModel(bunnyModel);
	bunny2->GetTransform().position = { 0.5f, 0.f, 0.f }; // Position it to the right of the light
	bunny2->GetTransform().rotation = { 0.f, 180.f, 0.f };
	bunny2->GetTransform().scale = { 0.5f, -0.5f, 0.5f };

	// Light in the middle of the two bunnies
	//GameObject pointLightComponent = GameObject::MakePointLight(0.2f);
	//pointLightComponent.GetTransform().position = { 0.f, -0.5f, 0.f }; // Slightly above ground level
	//pointLightComponent.SetColor({ 1.f, 0.f, 0.f });

	// Example vase and floor objects (optional, retained for context)
	const std::shared_ptr<ili::Model> vase = ili::Model::CreateModelFromFile(m_Device, "Assets/Models/flat_vase.obj");
	auto vaseObject = m_pCurrentScene->CreateGameObject();
	vaseObject->SetModel(vase);
	vaseObject->GetTransform().position = { 1.0f, 0.f, 0.0f };
	vaseObject->GetTransform().rotation = { 0.f, 0.f, 0.f };
	vaseObject->GetTransform().scale = { 1.f, 0.5f, 1.f };

	const std::shared_ptr<ili::Model> floor = ili::Model::CreateModelFromFile(m_Device, "Assets/Models/quad.obj");
	auto floorObject = m_pCurrentScene->CreateGameObject();
	floorObject->SetModel(floor);
	floorObject->GetTransform().position = { 0.5f, 0.f, 0.0f };
	floorObject->GetTransform().rotation = { 0.f, 0.f, 0.f };
	floorObject->GetTransform().scale = { 3.f, 0.5f, 3.f };
}


void IliadSampleProject::InitializeGame()
{
	auto mainScene = m_SceneManager.CreateScene("main");
	m_pCurrentScene = mainScene;

	LoadModels(*m_Device, m_pCurrentScene);

}