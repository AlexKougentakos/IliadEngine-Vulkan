#pragma once
#include "SceneGraph/Scene.h"

class IliadSampleScene : public ili::Scene
{
public:
	IliadSampleScene() = default;
	virtual ~IliadSampleScene() override = default;

	IliadSampleScene(const IliadSampleScene& other) = delete;
	IliadSampleScene(IliadSampleScene&& other) noexcept = delete;
	IliadSampleScene& operator=(const IliadSampleScene& other) = delete;
	IliadSampleScene& operator=(IliadSampleScene&& other) noexcept = delete;

	virtual void Initialize() override;

protected:
	virtual void Update(float deltaTime) override;
};
