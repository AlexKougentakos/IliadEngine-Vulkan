#pragma once

#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include "Graphics/Texture.h"
#include "Graphics/Device.h"
#include "Graphics/Descriptors.h"

namespace ili
{
    class Material
    {
    public:
        Material(Device& device, VkDescriptorSetLayout descriptorSetLayout);
        ~Material();

        // Disable copy constructors
        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;

        // Albedo (Diffuse)
        void SetAlbedoMap(const std::shared_ptr<Texture>& albedoMap);
        void SetAlbedoColor(const glm::vec3& color);
        std::shared_ptr<Texture> GetAlbedoMap() const;
        glm::vec3 GetAlbedoColor() const;

        // Normal Map
        void SetNormalMap(const std::shared_ptr<Texture>& normalMap);
        std::shared_ptr<Texture> GetNormalMap() const;

        // Metallic
        void SetMetallicMap(const std::shared_ptr<Texture>& metallicMap);
        void SetMetallicValue(float value);
        std::shared_ptr<Texture> GetMetallicMap() const;
        float GetMetallicValue() const;

        // Roughness
        void SetRoughnessMap(const std::shared_ptr<Texture>& roughnessMap);
        void SetRoughnessValue(float value);
        std::shared_ptr<Texture> GetRoughnessMap() const;
        float GetRoughnessValue() const;

        // Ambient Occlusion (AO)
        void SetAOMap(const std::shared_ptr<Texture>& aoMap);
        void SetAOValue(float value);
        std::shared_ptr<Texture> GetAOMap() const;
        float GetAOValue() const;

        // Descriptor set handling
        VkDescriptorSet GetDescriptorSet() const;
        void UpdateDescriptorSet();

    private:
        Device& m_Device;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        VkDescriptorSet m_DescriptorSet;
        VkBuffer m_UniformBuffer;
        VkDeviceMemory m_UniformBufferMemory;

        // Texture maps
        std::shared_ptr<Texture> m_AlbedoMap;
        std::shared_ptr<Texture> m_NormalMap;
        std::shared_ptr<Texture> m_MetallicMap;
        std::shared_ptr<Texture> m_RoughnessMap;
        std::shared_ptr<Texture> m_AOMap;

        void CreateUniformBuffer();
        void UpdateUniformBuffer();
        void CreateDescriptorSet();
    };
}
