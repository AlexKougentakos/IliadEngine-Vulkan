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
        void renderGameObjects(const FrameInfo& frameInfo, const std::vector<std::unique_ptr<GameObject>>& gameObjects);
    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);
        Device& lveDevice;
        std::unique_ptr<Pipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;
        std::unique_ptr<DescriptorSetLayout> renderSystemLayout;
    };
}