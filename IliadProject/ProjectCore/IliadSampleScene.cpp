#include "IliadSampleScene.h"

#include "Core/ContentLoader.h"
#include "Core/PointLight.h"
#include "SceneGraph/ModelComponent.h"
#include <glm/gtc/constants.hpp>
#include <memory>
#include <vector>

void IliadSampleScene::Initialize()
{
    // Load models
    const std::shared_ptr<ili::Model> bunnyModel = ili::ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/viking_room.obj");
    //const std::shared_ptr<ili::Model> vase = ili::ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/bg3_uw.obj");
    const std::shared_ptr<ili::Model> floor = ili::ContentLoader::GetInstance().LoadModelFromFile("Assets/Models/quad.obj");

    auto normal = ili::ContentLoader::GetInstance().LoadTextureFromFile("Assets/Textures/normal.png");
    auto rough = ili::ContentLoader::GetInstance().LoadTextureFromFile("Assets/Textures/rough.jpeg");
    auto colour = ili::ContentLoader::GetInstance().LoadTextureFromFile("Assets/Textures/colour.jpeg");
    auto metallic = ili::ContentLoader::GetInstance().LoadTextureFromFile("Assets/Textures/metallic.jpeg");
    auto ao = ili::ContentLoader::GetInstance().LoadTextureFromFile("Assets/Textures/ao.jpeg");



    auto texture2 = ili::ContentLoader::GetInstance().LoadTextureFromFile("Assets/Textures/bg3.png");

    // Create a single bunny at the center
    //auto bunny = CreateGameObject<ili::GameObject>();
    //auto modelComponent = bunny->AddComponent<ili::ModelComponent>();
    //modelComponent->SetModel(vase);
    //modelComponent->SetDiffuseMap(texture2);
    //bunny->GetTransform()->SetPosition({ 0.f, -1.8f, 0.f }); // Center
    //bunny->GetTransform()->SetRotationDegrees({ 0.f, 180.f, 180.f });
    //bunny->GetTransform()->SetScale({ 0.02f, 0.02f, 0.02f });

    // Define 5 unique light colors
    const std::vector<glm::vec3> lightColors =
    {
        {1.f, 0.f, 0.f}, // Red
        {0.f, 1.f, 0.f}, // Green
        {0.f, 0.f, 1.f}, // Blue
        {1.f, 1.f, 0.f}, // Yellow
        {1.f, 0.f, 1.f}  // Magenta
        // Removed Cyan to have 5 colors
    };

    constexpr float radius = 1.25f; // Distance from the bunny

    // Create top and bottom lights
    for (size_t i = 0; i < lightColors.size(); ++i)
    {
        // Top Light
        {
            const auto pointLight = CreatePointLight(.75f, 0.1f, lightColors[i]);
            const float angle = static_cast<float>(i) / lightColors.size() * 2.0f * glm::pi<float>();
            pointLight->GetTransform()->SetPosition({
                radius * std::cos(angle),
                -1.0f, // Top light height
                radius * std::sin(angle)
                });
            pointLight->SetColor(lightColors[i]);
            movingLights.push_back(MovingLight{ pointLight, angle, radius, 1}); // Clockwise
        }

        // Bottom Light (Duplicate with opposite direction)
        {
            const auto pointLight = CreatePointLight(0.6f, 0.1f, lightColors[i]);
            const float angle = static_cast<float>(i) / lightColors.size() * 2.0f * glm::pi<float>();
            pointLight->GetTransform()->SetPosition({
                radius * std::cos(angle),
                -3.75f, // Bottom light height
                radius * std::sin(angle)
                });
            pointLight->SetColor(lightColors[i]);
            movingLights.push_back({ pointLight, angle, radius, -1 }); // Counter-clockwise
        }
    }

    // Create the floor
    auto floorObject = CreateGameObject<ili::GameObject>();
    auto model = floorObject->AddComponent<ili::ModelComponent>();
    model->SetModel(floor);
    //model->SetDiffuseMap(texture);
	model->SetNormalMap(normal);
	model->SetRoughnessMap(rough);
	model->SetMetallicMap(metallic);
	model->SetDiffuseMap(colour);
	model->SetAOMap(ao);
    floorObject->GetTransform()->SetScale({ 5.f, 1.f, 5.f });
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
