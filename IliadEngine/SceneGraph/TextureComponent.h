#pragma once
#include <string>

#include "BaseComponent.h"

namespace ili
{

	class TextureComponent final : public BaseComponent
	{
	public:
		TextureComponent(unsigned int id, const std::string& texturePath);
		virtual ~TextureComponent() override = default;

		TextureComponent(const TextureComponent&) = delete;
		TextureComponent(TextureComponent&&) = delete;
		TextureComponent& operator=(const TextureComponent&) = delete;
		TextureComponent& operator=(TextureComponent&&) = delete;

		void SetTexturePath(const std::string& texturePath) { m_TexturePath = texturePath; }
		std::string GetTexturePath() const { return m_TexturePath; }

	private:
		std::string m_TexturePath{};
	};
}
