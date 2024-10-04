#include "TextureRenderSystem.h"

#include <glm/glm.hpp>
// std
#include <array>
#include <cassert>
#include <stdexcept>

#include "SceneGraph/ModelComponent.h"

namespace ili 
{
    struct TexturePushConstantData 
    {
        glm::mat4 modelMatrix{ 1.f };
        glm::mat4 normalMatrix{ 1.f };
    };

    TextureRenderSystem::TextureRenderSystem(
        Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
        : m_Device{ device } {
        CreatePipelineLayout(globalSetLayout);
        CreatePipeline(renderPass);
    }
    TextureRenderSystem::~TextureRenderSystem() {
        vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
    }
    void TextureRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(TexturePushConstantData);
        m_RenderSystemLayout =
            DescriptorSetLayout::Builder(m_Device)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .Build();

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts
        {
            globalSetLayout,
            m_RenderSystemLayout->GetDescriptorSetLayout() };
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }
    void TextureRenderSystem::CreatePipeline(VkRenderPass renderPass) 
    {
        assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
        PipelineConfigInfo pipelineConfig{};
        Pipeline::GetDefaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_PipelineLayout;
        m_pPipeline = std::make_unique<Pipeline>(
            m_Device,
            "Assets/CompiledShaders/texture_shader.vert.spv",
            "Assets/CompiledShaders/texture_shader.frag.spv",
            pipelineConfig);
    }
    void TextureRenderSystem::RenderGameObjects(const FrameInfo& frameInfo, const std::vector<std::unique_ptr<GameObject>>& gameObjects) 
    {
        m_pPipeline->Bind(frameInfo.commandBuffer);
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_PipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);
        for (const auto& gameObject : gameObjects)
        {
            const auto modelComponent = gameObject->GetComponent<ModelComponent>();
            // skip objects that don't have both a model and texture
            if (!modelComponent || !modelComponent->GetDiffuseMap()) continue;

            // writing descriptor set each frame can slow performance
            // would be more efficient to implement some sort of caching
            auto imageInfo = modelComponent->GetDiffuseMap()->GetImageInfo();
            VkDescriptorSet descriptorSet1;
            DescriptorWriter(*m_RenderSystemLayout, frameInfo.frameDescriptorPool)
                .WriteImage(0, &imageInfo)
                .Build(descriptorSet1);
            vkCmdBindDescriptorSets(
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_PipelineLayout,
                1,  // first set
                1,  // set count
                &descriptorSet1,
                0,
                nullptr);
            TexturePushConstantData push{};
            push.modelMatrix = gameObject->GetTransform()->GetMatrix();
            push.normalMatrix = gameObject->GetTransform()->GetNormalMatrix();
            vkCmdPushConstants(
                frameInfo.commandBuffer,
                m_PipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(TexturePushConstantData),
                &push);
            gameObject->GetComponent<ModelComponent>()->GetModel()->Bind(frameInfo.commandBuffer);
            gameObject->GetComponent<ModelComponent>()->GetModel()->Draw(frameInfo.commandBuffer);
        }
    }
}
