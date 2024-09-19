#pragma once

#include "Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <tiny_obj_loader.h>

namespace ili
{
	class Model
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal;
			glm::vec2 texCoord;
			
			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void LoadModel(const std::string& filepath);
		};

		Model(Device& device, const Builder& builder);
		~Model();

		Model(const Model& other) = delete;
		Model(Model&& other) noexcept = delete;
		Model& operator=(const Model& other) = delete;
		Model& operator=(Model&& other) noexcept = delete;

		static std::unique_ptr<Model> CreateModelFromFile(Device& device, const std::string& filepath);

		void Bind(VkCommandBuffer commandBuffer) const;
		void Draw(VkCommandBuffer commandBuffer) const;

	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);

		Device& m_Device;

		VkBuffer m_VertexBuffer{};
		VkDeviceMemory m_VertexBufferMemory{};
		uint32_t m_VertexCount{};

		bool m_HasIndexBuffer{false};
		VkBuffer m_IndexBuffer{};
		VkDeviceMemory m_IndexBufferMemory{};
		uint32_t m_IndexCount{};

	};

}
