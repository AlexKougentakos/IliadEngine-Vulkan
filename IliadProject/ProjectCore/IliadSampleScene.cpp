#include "IliadSampleScene.h"

#include "Core/ContentLoader.h"
#include "Core/PointLight.h"
#include "SceneGraph/ModelComponent.h"
#include <glm/gtc/constants.hpp>
#include <memory>
#include <vector>

void IliadSampleScene::Initialize()
{
    // Clear existing scene (if necessary)
    // This depends on your engine's implementation
    // For example:
    // ClearScene();

    // Load models
    const std::shared_ptr<ili::Model> planeModel = ili::ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/quad.obj");
    const std::shared_ptr<ili::Model> sphereModel = ili::ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/sphere.obj");

 //   auto go1 = CreateGameObject<ili::GameObject>();
	//go1->AddComponent<ili::ModelComponent>(planeModel);
	//go1->GetTransform()->GetScale() = { 20.f, 1.f, 20.f };

	//auto go2 = CreateGameObject<ili::GameObject>();
	//go2->AddComponent<ili::ModelComponent>(planeModel);
	//go2->GetTransform()->GetScale() = { 20.f, 1.f, 20.f };
 //   go2->GetTransform()->SetRotationDegrees(glm::vec3{180.f, 0.f, 0.f});
	//go2->GetTransform()->SetPosition(glm::vec3{ 0.f, -2.f, 0.f });

	//auto go3 = CreateGameObject<ili::GameObject>();
	//go3->AddComponent<ili::ModelComponent>(planeModel);
	//go3->GetTransform()->GetScale() = { 20.f, 1.f, 20.f };
	//go3->GetTransform()->SetRotationDegrees(glm::vec3{ 0.f, 0.f, 90.f });
	//go3->GetTransform()->SetPosition(glm::vec3{ 1.f, -1.f, 0.f });

	//auto go4 = CreateGameObject<ili::GameObject>();
	//go4->AddComponent<ili::ModelComponent>(planeModel);
	//go4->GetTransform()->GetScale() = { 20.f, 1.f, 20.f };
	//go4->GetTransform()->SetRotationDegrees(glm::vec3{ 0.f, 0.f, 90.f });
	//go4->GetTransform()->SetPosition(glm::vec3{ -1.f, -1.f, 0.f });

	//auto go5 = CreateGameObject<ili::GameObject>();
	//go5->AddComponent<ili::ModelComponent>(planeModel);
	//go5->GetTransform()->GetScale() = { 20.f, 1.f, 20.f };
	//go5->GetTransform()->SetRotationDegrees(glm::vec3{ 90.f, 0.f, 0.f });
	//go5->GetTransform()->SetPosition(glm::vec3{ 0.f, -1.f, 1.f });

	auto go6 = CreateGameObject<ili::GameObject>();

	auto goldMat = std::make_shared<ili::Material>();
	//goldMat->SetAlbedo({ 1.f, 0.86f, 0.67f });
	goldMat->SetAlbedo({ 1.f, 1.f, 1.f });
	goldMat->SetMetallic(1.f);
	goldMat->SetRoughness(0.3f);

	go6->AddComponent<ili::ModelComponent>(sphereModel)->SetMaterial(goldMat);
	go6->GetTransform()->SetPosition(glm::vec3{ 0.f, -1.f, 0.f });
	go6->GetTransform()->SetScale({ 0.3, 0.3, 0.3 });

	auto light = CreatePointLight(2.f, 0.1f, {1.f, 0.f, 0.f});
	light->GetTransform()->SetPosition(glm::vec3{ 0.f, -2.f, -2.f });
}


void IliadSampleScene::Update(float deltaTime)
{
    // Update positions of moving lights
    for (auto& ml : movingLights)
    {
        // Update angle based on speed, deltaTime, and direction
        ml.angle += ml.direction * deltaTime;
        if (ml.angle > 2.0f * glm::pi<float>())
            ml.angle -= 2.0f * glm::pi<float>();
        else if (ml.angle < 0.f)
            ml.angle += 2.0f * glm::pi<float>();

        // Calculate new position
        float x = ml.radius * std::cos(ml.angle);
        float z = ml.radius * std::sin(ml.angle);
        // Determine y-position based on current light's y
        float currentY = ml.lightComponent->GetTransform()->GetPosition().y;
        ml.lightComponent->GetTransform()->SetPosition({ x, currentY, z });
    }
}
