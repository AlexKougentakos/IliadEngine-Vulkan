#pragma once

#include <memory>
#include <string>
#include <glm/vec3.hpp>

#include "Core/ContentLoader.h"
#include "Graphics/Texture.h"
#include "Graphics/Device.h"
#include "Graphics/Descriptors.h"

namespace ili
{

    //I don't know if I like this as a class. It's just a container.
	// I will leave it for now in case I want to add more functionality to it.
    //todo: come back to this
    class Material
    {
    public:
        Material();
        ~Material() = default;

        // Disable copy constructors
        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;
		Material(Material&&) = delete;
		Material& operator=(Material&&) = delete;

        // Albedo (Diffuse)
		void SetAlbedo(const std::shared_ptr<Texture>& albedoMap) { m_AlbedoMap = albedoMap; }
        void SetAlbedo(const glm::vec3& color) { m_AlbedoMap = ContentLoader::GetInstance().CreateTextureFromColor({ color, 1.f }); }

        // Normal Map
		void SetNormal(const std::shared_ptr<Texture>& normalMap) { m_NormalMap = normalMap; }

        // Metallic
		void SetMetallic(const std::shared_ptr<Texture>& metallicMap) { m_MetallicMap = metallicMap; }
		void SetMetallic(float value) { m_MetallicMap = ContentLoader::GetInstance().CreateTextureFromColor({ value, value, value, 1.f }); }

        // Roughness
		void SetRoughness(const std::shared_ptr<Texture>& roughnessMap) { m_RoughnessMap = roughnessMap; }
		void SetRoughness(float value) { m_RoughnessMap = ContentLoader::GetInstance().CreateTextureFromColor({ value, value, value, 1.f }); }
        
        // Ambient Occlusion (AO)
		void SetAO(const std::shared_ptr<Texture>& aoMap) { m_AOMap = aoMap; }
		void SetAO(float value) { m_AOMap = ContentLoader::GetInstance().CreateTextureFromColor({ value, value, value, 1.f }); }

        std::shared_ptr<Texture> GetAlbedoMap() const { return m_AlbedoMap; }
		std::shared_ptr<Texture> GetNormalMap() const { return m_NormalMap; }
		std::shared_ptr<Texture> GetMetallicMap() const { return m_MetallicMap; }
		std::shared_ptr<Texture> GetRoughnessMap() const { return m_RoughnessMap; }
		std::shared_ptr<Texture> GetAOMap() const { return m_AOMap; }

    private:
        // Texture maps
        std::shared_ptr<Texture> m_AlbedoMap{};
        std::shared_ptr<Texture> m_NormalMap{};
        std::shared_ptr<Texture> m_MetallicMap{};
        std::shared_ptr<Texture> m_RoughnessMap{};
        std::shared_ptr<Texture> m_AOMap{};
    };
}
