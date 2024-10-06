#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// std
#include <cmath>
#include <stdexcept>

namespace ili
{
    Texture::Texture(Device& device, const std::string& textureFilepath)
        : m_Device{ device }
    {
        CreateTextureImage(textureFilepath);
        CreateTextureImageView(VK_IMAGE_VIEW_TYPE_2D);
        CreateTextureSampler();
        UpdateDescriptor();
    }

    Texture::Texture(
        Device& device,
        VkFormat format,
        VkExtent3D extent,
        VkImageUsageFlags usage,
        VkSampleCountFlagBits sampleCount)
        : m_Device{ device },
        m_Format{ format },
        m_Extent{ extent },
        m_MipLevels{ 1 },
        m_LayerCount{ 1 }
    {
        VkImageAspectFlags aspectMask = 0;
        VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Initialize with default value

        if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
        else if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }
        else if (usage & VK_IMAGE_USAGE_SAMPLED_BIT)
        {
            aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        else
        {
            throw std::runtime_error("Unsupported image usage flags!");
        }

        // Don't like this, should I be using an image array instead of multiple images?
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = m_Format;
        imageInfo.extent = m_Extent;
        imageInfo.mipLevels = m_MipLevels;
        imageInfo.arrayLayers = m_LayerCount;
        imageInfo.samples = sampleCount;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = usage;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        m_Device.CreateImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_pTextureImage,
            m_pTextureImageMemory);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_Format;
        viewInfo.subresourceRange = {};
        viewInfo.subresourceRange.aspectMask = aspectMask;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.image = m_pTextureImage;

        if (vkCreateImageView(m_Device.GetDevice(), &viewInfo, nullptr, &m_pTextureImageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }

        // Sampler should be separated out
        if (usage & VK_IMAGE_USAGE_SAMPLED_BIT)
        {
            // Create sampler to sample from the attachment in the fragment shader
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            samplerInfo.addressModeV = samplerInfo.addressModeU;
            samplerInfo.addressModeW = samplerInfo.addressModeU;
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.maxAnisotropy = 1.0f;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = 1.0f;
            samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

            if (vkCreateSampler(m_Device.GetDevice(), &samplerInfo, nullptr, &m_pTextureSampler) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create sampler!");
            }

            VkImageLayout samplerImageLayout;
            if (imageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            {
                samplerImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
            else if (imageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            {
                samplerImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            }
            else if (imageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            {
                samplerImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
            else
            {
                samplerImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }

            // Set the descriptor
            m_Descriptor.sampler = m_pTextureSampler;
            m_Descriptor.imageView = m_pTextureImageView;
            m_Descriptor.imageLayout = samplerImageLayout;
        }
    }

    Texture::~Texture()
    {
        vkDestroySampler(m_Device.GetDevice(), m_pTextureSampler, nullptr);
        vkDestroyImageView(m_Device.GetDevice(), m_pTextureImageView, nullptr);
        vkDestroyImage(m_Device.GetDevice(), m_pTextureImage, nullptr);
        vkFreeMemory(m_Device.GetDevice(), m_pTextureImageMemory, nullptr);
    }

    std::unique_ptr<Texture> Texture::CreateTextureFromFile(
        Device& device, const std::string& filepath)
    {
        return std::make_unique<Texture>(device, filepath);
    }

    void Texture::UpdateDescriptor()
    {
        m_Descriptor.sampler = m_pTextureSampler;
        m_Descriptor.imageView = m_pTextureImageView;
        m_Descriptor.imageLayout = m_TextureLayout;
    }

    void Texture::UploadData(const void* data, VkDeviceSize size)
    {
        // Create staging buffer
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        m_Device.CreateBuffer(
            size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        // Copy data to staging buffer
        void* bufferData;
        vkMapMemory(m_Device.GetDevice(), stagingBufferMemory, 0, size, 0, &bufferData);
        memcpy(bufferData, data, static_cast<size_t>(size));
        vkUnmapMemory(m_Device.GetDevice(), stagingBufferMemory);

        // Transition image layout to transfer destination
        m_Device.TransitionImageLayout(
            m_pTextureImage,
            m_Format,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            m_MipLevels,
            m_LayerCount);

        // Copy buffer data to image
        m_Device.CopyBufferToImage(
            stagingBuffer,
            m_pTextureImage,
            m_Extent.width,
            m_Extent.height,
            m_LayerCount);

        // Transition image layout to shader read
        m_Device.TransitionImageLayout(
            m_pTextureImage,
            m_Format,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            m_MipLevels,
            m_LayerCount);

        m_TextureLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        // Clean up staging resources
        vkDestroyBuffer(m_Device.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(m_Device.GetDevice(), stagingBufferMemory, nullptr);
    }

    void Texture::CreateTextureImage(const std::string& filepath)
    {
        int texWidth, texHeight, texChannels;
        // stbi_set_flip_vertically_on_load(1);  // todo determine why texture coordinates are flipped
        stbi_uc* pixels =
            stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        // mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
        m_MipLevels = 1;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        m_Device.CreateBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(m_Device.GetDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(m_Device.GetDevice(), stagingBufferMemory);
        stbi_image_free(pixels);

        m_Format = VK_FORMAT_R8G8B8A8_SRGB;
        m_Extent = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 };

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent = m_Extent;
        imageInfo.mipLevels = m_MipLevels;
        imageInfo.arrayLayers = m_LayerCount;
        imageInfo.format = m_Format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        m_Device.CreateImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_pTextureImage,
            m_pTextureImageMemory);

        m_Device.TransitionImageLayout(
            m_pTextureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            m_MipLevels,
            m_LayerCount);

        m_Device.CopyBufferToImage(
            stagingBuffer,
            m_pTextureImage,
            static_cast<uint32_t>(texWidth),
            static_cast<uint32_t>(texHeight),
            m_LayerCount);

        // Comment this out if using mips
        m_Device.TransitionImageLayout(
            m_pTextureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            m_MipLevels,
            m_LayerCount);

        // If we generate mip maps then the final image will already be READ_ONLY_OPTIMAL
        // m_Device.GenerateMipmaps(m_pTextureImage, m_Format, texWidth, texHeight, m_MipLevels);
        m_TextureLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        vkDestroyBuffer(m_Device.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(m_Device.GetDevice(), stagingBufferMemory, nullptr);
    }

    void Texture::CreateTextureImageView(VkImageViewType viewType)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_pTextureImage;
        viewInfo.viewType = viewType;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = m_MipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = m_LayerCount;

        if (vkCreateImageView(m_Device.GetDevice(), &viewInfo, nullptr, &m_pTextureImageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    void Texture::CreateTextureSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = 16.0f;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        // These fields are useful for percentage closer filtering for shadow maps
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(m_MipLevels);

        if (vkCreateSampler(m_Device.GetDevice(), &samplerInfo, nullptr, &m_pTextureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void Texture::TransitionLayout(
        VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_pTextureImage;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = m_MipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = m_LayerCount;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (m_Format == VK_FORMAT_D32_SFLOAT_S8_UINT || m_Format == VK_FORMAT_D24_UNORM_S8_UINT)
            {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (
            oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (
            oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (
            oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
            newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask =
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else if (
            oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
            newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            // This ensures that any commands acting on color output or after (dstStage)
            // that need read or write access to a resource
            // must wait until all previous read accesses in fragment shader
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            barrier.dstAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage,
            destinationStage,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);
    }
}
