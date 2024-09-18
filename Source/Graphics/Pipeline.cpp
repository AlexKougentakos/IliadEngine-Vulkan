#include "Pipeline.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "Model.h"

namespace ili
{
	Pipeline::Pipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath,
		const PipelineConfigInfo& info) : m_Device(device)
	{
		CreateGraphicsPipeline(vertFilepath, fragFilepath, info);
	}

	Pipeline::~Pipeline()
	{
		vkDestroyShaderModule(m_Device.GetDevice(), m_VertexShaderModule, nullptr);
		vkDestroyShaderModule(m_Device.GetDevice(), m_FragmentShaderModule, nullptr);
		vkDestroyPipeline(m_Device.GetDevice(), m_GraphicsPipeline, nullptr);
	}

	std::vector<char> Pipeline::ReadFile(const std::string& filepath)
	{
		// The ate flag means that we are starting at the end of the file
		// The binary flag just means that we are reading the file in binary mode
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file: " + filepath);
		}

		// Tellg gets the position of the current character in the input stream
		// This is the size of the file because we are starting at the end
		const size_t fileSize = (size_t)file.tellg();

		std::vector<char> buffer(fileSize);

		// Seekg moves the position of the current character in the input stream
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void Pipeline::CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo)
	{
		std::string test = std::filesystem::current_path().string();

		const auto vertCode = ReadFile(vertFilepath);
		const auto fragCode = ReadFile(fragFilepath);

		CreateShaderModule(vertCode, &m_VertexShaderModule);
		CreateShaderModule(fragCode, &m_FragmentShaderModule);

		VkPipelineShaderStageCreateInfo shaderStages[2]{};
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = m_VertexShaderModule;
		shaderStages[0].pName = "main"; //The name of the entry point function in the VERTEX shader
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = m_FragmentShaderModule;
        shaderStages[1].pName = "main"; //The name of the entry point function in the shader
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

		const auto bindingDescriptions = Model::Vertex::GetBindingDescriptions();
		const auto attributeDescriptions = Model::Vertex::GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO; // Specify the type of viewport state structure
		viewportInfo.viewportCount = 1; // Use a single viewport
		viewportInfo.pViewports = &configInfo.viewport; // Reference the configured viewport
		viewportInfo.scissorCount = 1; // Use a single scissor rectangle
		viewportInfo.pScissors = &configInfo.scissor; // Reference the configured scissor rectangle

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = nullptr;

		pipelineInfo.layout = configInfo.pipelineLayout;
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(m_Device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
        {
			throw std::runtime_error("Failed to create graphics pipeline!");
        }

    }

	void Pipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_Device.GetDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module!");
		}
	}

    PipelineConfigInfo Pipeline::GetDefaultPipelineConfigInfo(uint32_t width, uint32_t height)
    {
        PipelineConfigInfo configInfo{}; // Initialize pipeline configuration with default values

        configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO; // Specify the type of input assembly structure
        configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // Define how primitives are assembled (as triangles)
        configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE; // Disable primitive restart for strip topologies

        configInfo.viewport.x = 0.0f; // Set viewport's X coordinate start
        configInfo.viewport.y = 0.0f; // Set viewport's Y coordinate start
        configInfo.viewport.width = static_cast<float>(width); // Set viewport width based on input
        configInfo.viewport.height = static_cast<float>(height); // Set viewport height based on input
        configInfo.viewport.minDepth = 0.0f; // Define the nearest depth value mapped to the viewport
        configInfo.viewport.maxDepth = 1.0f; // Define the farthest depth value mapped to the viewport

        configInfo.scissor.offset = { 0, 0 }; // Set scissor rectangle starting at top-left
        configInfo.scissor.extent = { width, height }; // Define scissor rectangle size matching viewport

        configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO; // Specify the type of rasterization state structure
        configInfo.rasterizationInfo.depthClampEnable = VK_FALSE; // Disable depth clamping to keep fragments within depth range
        configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE; // Enable rasterization stage
        configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL; // Fill polygons rather than drawing wireframes
        configInfo.rasterizationInfo.lineWidth = 1.0f; // Set line width for rasterization
        configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE; // Disable face culling
        configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; // Define front faces as clockwise-wound
        configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE; // Disable depth bias
        configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Set constant depth bias factor
        configInfo.rasterizationInfo.depthBiasClamp = 0.0f; // Set maximum depth bias clamp
        configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f; // Set slope-scaled depth bias factor

        configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO; // Specify the type of multisample state structure
        configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE; // Disable sample shading
        configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // Use single-sample rasterization
        configInfo.multisampleInfo.minSampleShading = 1.0f; // Set minimum sample shading to full coverage
        configInfo.multisampleInfo.pSampleMask = nullptr; // No sample mask applied
        configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Disable alpha-to-coverage
        configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE; // Disable alpha-to-one

        configInfo.colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT; // Enable writing to all color channels
        configInfo.colorBlendAttachment.blendEnable = VK_FALSE; // Disable color blending
        configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Set source blend factor
        configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Set destination blend factor
        configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Define color blend operation
        configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Set source alpha blend factor
        configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Set destination alpha blend factor
        configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Define alpha blend operation

        configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO; // Specify the type of color blend state structure
        configInfo.colorBlendInfo.logicOpEnable = VK_FALSE; // Disable logical operations on color data
        configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Set logical operation to copy (unused)
        configInfo.colorBlendInfo.attachmentCount = 1; // Use one color blend attachment
        configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment; // Reference the color blend attachment
        configInfo.colorBlendInfo.blendConstants[0] = 0.0f; // Initialize blend constant for red
        configInfo.colorBlendInfo.blendConstants[1] = 0.0f; // Initialize blend constant for green
        configInfo.colorBlendInfo.blendConstants[2] = 0.0f; // Initialize blend constant for blue
        configInfo.colorBlendInfo.blendConstants[3] = 0.0f; // Initialize blend constant for alpha

        configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO; // Specify the type of depth stencil state structure
        configInfo.depthStencilInfo.depthTestEnable = VK_TRUE; // Enable depth testing to handle fragment visibility
        configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE; // Allow writing to the depth buffer
        configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS; // Use less-than comparison for depth testing
        configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE; // Disable depth bounds testing
        configInfo.depthStencilInfo.minDepthBounds = 0.0f; // Set minimum depth bound (unused)
        configInfo.depthStencilInfo.maxDepthBounds = 1.0f; // Set maximum depth bound (unused)
        configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE; // Disable stencil testing
        configInfo.depthStencilInfo.front = {}; // Initialize front stencil state (unused)
        configInfo.depthStencilInfo.back = {}; // Initialize back stencil state (unused)

        return configInfo; // Return the fully configured pipeline settings
    }

    void Pipeline::Bind(VkCommandBuffer commandBuffer)
    {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
    }
}
