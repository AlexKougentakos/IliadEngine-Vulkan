#include "Model.h"
#include "../Core/Utils.h"
#include <stdexcept>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>

namespace std
{
	template<>
	struct hash<ili::Model::Vertex>
	{
		size_t operator()(ili::Model::Vertex const& vertex) const noexcept
		{
			size_t seed = 0;
			ili::Utils::HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.texCoord);
			return seed;

		}
	};
}

namespace ili
{
	//
	//  VERTEX STRUCT
	//
	std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, position)});
		attributeDescriptions.push_back({ 1,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, color) });
		attributeDescriptions.push_back({ 2,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3,0,VK_FORMAT_R32G32_SFLOAT,offsetof(Vertex, texCoord) });

		return attributeDescriptions;
	}

	//
	//  BUILDER STRUCT
	//	

	void Model::Builder::LoadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib{};
		std::vector<tinyobj::shape_t> shapes{};
		std::vector<tinyobj::material_t> materials{};

		std::string warn{};
		std::string err{};

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{}; //Map of vertices to their index
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				if (index.vertex_index >= 0)
				{
					vertex.position = 
					{
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.color =
					{
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};
				}

				if (index.normal_index >= 0)
				{
					vertex.normal =
					{
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.texCoord =
					{
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (!uniqueVertices.contains(vertex))
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.emplace_back(uniqueVertices[vertex]);
			}
		}
	}

	Model::Model(Device& device, const Builder& builder)
		: m_Device(device)
	{
		CreateVertexBuffers(builder.vertices);
		CreateIndexBuffers(builder.indices);
	}

	Model::~Model()
	{
		
	}

	void Model::CreateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_VertexCount = static_cast<uint32_t>(vertices.size());

		assert(m_VertexCount >= 3 && "Vertex count must be at least 3");

		// This just calculates how much memory we need to allocate for all the vertices of the model
		const VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
		constexpr uint32_t vertexSize = sizeof(vertices[0]);


		Buffer stagingBuffer{ m_Device, vertexSize, m_VertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)(vertices.data()));

		m_pVertexBuffer = std::make_unique<Buffer>(m_Device, vertexSize, m_VertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_Device.CopyBuffer(stagingBuffer.GetBuffer(), m_pVertexBuffer->GetBuffer(), bufferSize);
	}

	void Model::CreateIndexBuffers(const std::vector<uint32_t>& indices)
	{
		m_IndexCount = static_cast<uint32_t>(indices.size());
		m_HasIndexBuffer = m_IndexCount > 0;

		if (!m_HasIndexBuffer) return;

		//Create a buffer on the GPU and allocate memory for it
		const VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;
		constexpr uint32_t indexSize = sizeof(indices[0]);

		Buffer stagingBuffer{ m_Device, indexSize, m_IndexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)(indices.data()));

		m_pIndexBuffer = std::make_unique<Buffer>(m_Device, indexSize, m_IndexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_Device.CopyBuffer(stagingBuffer.GetBuffer(), m_pIndexBuffer->GetBuffer(), bufferSize);
	}

	std::unique_ptr<Model> Model::CreateModelFromFile(Device& device, const std::string& filepath)
	{
		Builder builder{};
		builder.LoadModel(filepath);
		return std::make_unique<Model>(device, builder);
	}

	void Model::Bind(VkCommandBuffer commandBuffer) const
	{
		const VkBuffer buffers[] = { m_pVertexBuffer->GetBuffer()};
		constexpr VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (m_HasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, m_pIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void Model::Draw(VkCommandBuffer commandBuffer) const
	{
		if (m_HasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
		}
	}

	
}
