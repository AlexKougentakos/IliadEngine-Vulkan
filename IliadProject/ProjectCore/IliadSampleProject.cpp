#include "IliadSampleProject.h"
#include "IliadSampleScene.h"

void IliadSampleProject::OnGamePreparing()
{

}

void IliadSampleProject::InitializeGame()
{
	m_pCurrentScene = m_SceneManager.CreateScene<IliadSampleScene>("MainScene");
}