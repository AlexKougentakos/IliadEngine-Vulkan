#pragma once
#include "Graphics/Device.h"
// libs
#include <vulkan/vulkan.h>
// std
#include <memory>
#include <string>

namespace ili
{
    class Texture 
    {
    public:
        Texture(Device& device, const std::string& textureFilepath);
        Texture(
            Device& device,
            VkFormat format,
            VkExtent3D extent,
            VkImageUsageFlags usage,
            VkSampleCountFlagBits sampleCount);
        ~Texture();

        // Delete copy constructor and copy assignment
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Accessor functions with uppercase first letters
        VkImageView ImageView() const { return m_pTextureImageView; }
        VkSampler Sampler() const { return m_pTextureSampler; }
        VkImage GetImage() const { return m_pTextureImage; }
        VkImageView GetImageView() const { return m_pTextureImageView; }
        VkDescriptorImageInfo GetImageInfo() const { return m_Descriptor; }
        VkImageLayout GetImageLayout() const { return m_TextureLayout; }
        VkExtent3D GetExtent() const { return m_Extent; }
        VkFormat GetFormat() const { return m_Format; }

        // Member functions with uppercase first letters and brace on next line
        void UpdateDescriptor();
        void TransitionLayout(
            VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);
        static std::unique_ptr<Texture> CreateTextureFromFile(
            Device& device, const std::string& filepath);
    private:
        // Private member functions with uppercase first letters
        void CreateTextureImage(const std::string& filepath);
        void CreateTextureImageView(VkImageViewType viewType);
        void CreateTextureSampler();

        // Member variables with m_ prefix and appropriate pointer conventions
        VkDescriptorImageInfo m_Descriptor{};
        Device& m_Device;
        VkImage m_pTextureImage = VK_NULL_HANDLE;
        VkDeviceMemory m_pTextureImageMemory = VK_NULL_HANDLE;
        VkImageView m_pTextureImageView = VK_NULL_HANDLE;
        VkSampler m_pTextureSampler = VK_NULL_HANDLE;
        VkFormat m_Format = VK_FORMAT_UNDEFINED;
        VkImageLayout m_TextureLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        uint32_t m_MipLevels{ 1 };
        uint32_t m_LayerCount{ 1 };
        VkExtent3D m_Extent{ 0, 0, 0 };
    };
}
