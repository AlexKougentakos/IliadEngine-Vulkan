#include "Material.h"

ili::Material::Material()
{
	//todo: probably optimize this, because every time you create a new material it will create a new texture
	//Initialize all the texture maps to their default values
	m_AlbedoMap = ContentLoader::GetInstance().CreateTextureFromColor({ 1.f, 1.f, 1.f, 1.f });
	m_NormalMap = ContentLoader::GetInstance().CreateTextureFromColor({ 0.5f, 0.5f, 1.f, 1.f });
	m_MetallicMap = ContentLoader::GetInstance().CreateTextureFromColor({ 0.f, 0.f, 0.f, 1.f });
	m_RoughnessMap = ContentLoader::GetInstance().CreateTextureFromColor({ 0.5f, 0.5f, 0.5f, 1.f });
	m_AOMap = ContentLoader::GetInstance().CreateTextureFromColor({ 1.f, 1.f, 1.f, 1.f });
}
