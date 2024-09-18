#pragma once

#include "Device.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace ili
{
	class Model
	{
	public:
		struct Vertex
		{
			glm::vec2 position;
			
			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		Model(Device& device, const std::vector<Vertex>& vertices);
		~Model();

		Model(const Model& other) = delete;
		Model(Model&& other) noexcept = delete;
		Model& operator=(const Model& other) = delete;
		Model& operator=(Model&& other) noexcept = delete;

		void Bind(VkCommandBuffer commandBuffer) const;
		void Draw(VkCommandBuffer commandBuffer) const;

	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);

		Device& m_Device;
		VkBuffer m_VertexBuffer{};
		VkDeviceMemory m_VertexBufferMemory{};
		uint32_t m_VertexCount{};

	};

}
