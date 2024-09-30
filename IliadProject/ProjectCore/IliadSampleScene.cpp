#include "IliadSampleScene.h"

#include "Core/ContentLoader.h"
#include "Core/PointLight.h"

void IliadSampleScene::Initialize()
{
	const std::shared_ptr<ili::Model> bunnyModel = ili::ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/bunny.obj");

	auto bunny1 = CreateGameObject<ili::GameObject>();
	bunny1->SetModel(bunnyModel);
	bunny1->GetTransform()->position = { -0.5f, 0.f, 0.f };
	bunny1->GetTransform()->rotation = { 0.f, 180.f, 0.f };
	bunny1->GetTransform()->scale = { 0.5f, -0.5f, 0.5f };


	// Second bunny
	auto bunny2 = CreateGameObject<ili::GameObject>();
	bunny2->SetModel(bunnyModel);
	bunny2->GetTransform()->position = { 0.5f, 0.f, 0.f }; // Position it to the right of the light
	bunny2->GetTransform()->rotation = { 0.f, 180.f, 0.f };
	bunny2->GetTransform()->scale = { 0.5f, -0.5f, 0.5f };

	//todo: abstract the lighting objects
	//Light in the middle of the two bunnies
	auto pointLightComponent = CreatePointLight(1.f, 0.2f, { 1.f, 0.f, 0.f });
	pointLightComponent->GetTransform()->position = { 0.f, -0.5f, 0.f }; // Slightly above ground level
	pointLightComponent->SetColor({ 1.f, 0.f, 0.f });

	// Example vase and floor objects (optional, retained for context)
	const std::shared_ptr<ili::Model> vase = ili::ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/flat_vase.obj");;
	auto vaseObject = CreateGameObject<ili::GameObject>();
	vaseObject->SetModel(vase);
	vaseObject->GetTransform()->position = { 1.0f, 0.f, 0.0f };
	vaseObject->GetTransform()->rotation = { 0.f, 0.f, 0.f };
	vaseObject->GetTransform()->scale = { 1.f, 0.5f, 1.f };

	const std::shared_ptr<ili::Model> floor = ili::ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/quad.obj");
	auto floorObject = CreateGameObject<ili::GameObject>();
	floorObject->SetModel(floor);
	floorObject->GetTransform()->position = { 0.5f, 0.f, 0.0f };
	floorObject->GetTransform()->rotation = { 0.f, 0.f, 0.f };
	floorObject->GetTransform()->scale = { 3.f, 0.5f, 3.f };
}

void IliadSampleScene::Update(float deltaTime)
{

}