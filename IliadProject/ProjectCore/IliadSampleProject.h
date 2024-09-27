#pragma once

#include "Core/IliadGame.h"

class IliadSampleProject : public ili::IliadGame
{
public:
	IliadSampleProject() = default;
	virtual ~IliadSampleProject() override = default;

	IliadSampleProject(const IliadSampleProject& other) = delete;
	IliadSampleProject(IliadSampleProject&& other) noexcept = delete;
	IliadSampleProject& operator=(const IliadSampleProject& other) = delete;
	IliadSampleProject& operator=(IliadSampleProject&& other) noexcept = delete;

protected:
	virtual void OnGamePreparing() override;
	virtual void InitializeGame() override;
};