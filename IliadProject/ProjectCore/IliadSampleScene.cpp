#include "IliadSampleScene.h"

#include "Core/ContentLoader.h"
#include "Core/PointLight.h"
#include "SceneGraph/ModelComponent.h"

void IliadSampleScene::Initialize()
{
    // Load models
    const std::shared_ptr<ili::Model> bunnyModel = ili::ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/viking_room.obj");
    const std::shared_ptr<ili::Model> vase = ili::ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/flat_vase.obj");
    const std::shared_ptr<ili::Model> floor = ili::ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/quad.obj");

	auto texture = ili::ContentLoader::GetInstance().LoadTextureFromFile("Assets/Textures/cat.jpg");
	auto texture2 = ili::ContentLoader::GetInstance().LoadTextureFromFile("Assets/Textures/viking_room.png");


    //todo: create the texture here

    // Create a single bunny at the center
    auto bunny = CreateGameObject<ili::GameObject>();
    auto modelComponent = bunny->AddComponent<ili::ModelComponent>();
	modelComponent->SetModel(bunnyModel);
	modelComponent->SetDiffuseMap(texture2);
    bunny->GetTransform()->position = { 0.f, 0.f, 0.f }; // Center
    bunny->GetTransform()->rotation = { glm::radians < float> (90), glm::radians<float>(45), 0.f };
    bunny->GetTransform()->scale = { 2.5f, -2.5f, 2.5f };

    // Create static lights with different colors
    const std::vector<glm::vec3> lightColors = 
    {
        {1.f, 0.f, 0.f}, // Red
        {0.f, 1.f, 0.f}, // Green
        {0.f, 0.f, 1.f}, // Blue
        {1.f, 1.f, 0.f}, // Yellow
        {1.f, 0.f, 1.f}, // Magenta
        {0.f, 1.f, 1.f}  // Cyan
    };

    for (size_t i = 0; i < lightColors.size(); ++i)
    {
        const auto pointLight = CreatePointLight(0.6f, 0.1f, lightColors[i]);
        // Position lights around the bunny in a circle on the ground
        const float angle = static_cast<float>(i) / lightColors.size() * 2.0f * glm::pi<float>();
        constexpr  float radius = 1.0f; // Distance from the bunny
        pointLight->GetTransform()->position = {
            radius * std::cos(angle),
            -1.0f, // Slightly above ground
            radius * std::sin(angle)
        };
        pointLight->SetColor(lightColors[i]);
		movingLights.push_back({ pointLight, angle, radius });
    }

    auto floorObject = CreateGameObject<ili::GameObject>();
    floorObject->AddComponent<ili::ModelComponent>()->SetModel(floor);
    floorObject->GetTransform()->position = { 0.0f, 0.f, 0.0f };
    floorObject->GetTransform()->rotation = { 0.f, 0.f, 0.f };
    floorObject->GetTransform()->scale = { 5.f, 1.f, 5.f };
}

void IliadSampleScene::Update(float deltaTime)
{
    // Update positions of moving lights
    for (auto& ml : movingLights)
    {
        // Update angle based on speed and deltaTime
        ml.angle += deltaTime;
        if (ml.angle > 2.0f * glm::pi<float>())
            ml.angle -= 2.0f * glm::pi<float>();

        // Calculate new position
        float x = ml.radius * std::cos(ml.angle);
        float z = ml.radius * std::sin(ml.angle);
        ml.lightComponent->GetTransform()->position = { x, -1.f, z };
    }
}