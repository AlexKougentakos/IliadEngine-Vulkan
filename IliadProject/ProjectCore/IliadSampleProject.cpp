#include "IliadSampleProject.h"
#include "IliadSampleScene.h"

void IliadSampleProject::OnGamePreparing()
{

}

void LoadModels(ili::Device& m_Device, ili::Scene* m_pCurrentScene)
{

}


void IliadSampleProject::InitializeGame()
{
	m_pCurrentScene = m_SceneManager.CreateScene<IliadSampleScene>("MainScene");

	//LoadModels(*m_Device, m_pCurrentScene);
}