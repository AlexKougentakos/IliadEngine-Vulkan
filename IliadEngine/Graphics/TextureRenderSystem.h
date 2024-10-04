#pragma once
#include "SceneGraph/GameObject.h"
#include "Graphics/Descriptors.h"
#include "Graphics/Device.h"
#include "Graphics/Pipeline.h"
#include "Structs/FrameInfo.h"

#include <memory>
#include <vector>
namespace ili 
{
    class TextureRenderSystem 
    {
    public:
        TextureRenderSystem(
            Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~TextureRenderSystem();
        TextureRenderSystem(const TextureRenderSystem&) = delete;
        TextureRenderSystem& operator=(const TextureRenderSystem&) = delete;
        void RenderGameObjects(const FrameInfo& frameInfo, const std::vector<std::unique_ptr<GameObject>>& gameObjects);
    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void CreatePipeline(VkRenderPass renderPass);
        Device& m_Device;
        std::unique_ptr<Pipeline> m_pPipeline{};
        VkPipelineLayout m_PipelineLayout{};
        std::unique_ptr<DescriptorSetLayout> m_RenderSystemLayout;
    };
}