#include "Model.h"


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
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0; //This corresponds to the location specified in the vertex shader
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1; //This corresponds to the location specified in the vertex shader
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT; //Vec3 so it has 3 components
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}



	Model::Model(Device& device, const Builder& builder)
		: m_Device(device)
	{
		CreateVertexBuffers(builder.vertices);
		CreateIndexBuffers(builder.indices);
	}

	Model::~Model()
	{
		vkDestroyBuffer(m_Device.GetDevice(), m_VertexBuffer, nullptr);
		vkFreeMemory(m_Device.GetDevice(), m_VertexBufferMemory, nullptr);

		if (m_HasIndexBuffer)
		{
			vkDestroyBuffer(m_Device.GetDevice(), m_IndexBuffer, nullptr);
			vkFreeMemory(m_Device.GetDevice(), m_IndexBufferMemory, nullptr);
		}
	}

	void Model::CreateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_VertexCount = static_cast<uint32_t>(vertices.size());

		assert(m_VertexCount >= 3 && "Vertex count must be at least 3");

		// This just calculates how much memory we need to allocate for all the vertices of the model
		const VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;

		VkBuffer stagingBuffer{};
		VkDeviceMemory stagingBufferMemory{};
		m_Device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(m_Device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_Device.GetDevice(), stagingBufferMemory);

		m_Device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

		m_Device.CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

		vkDestroyBuffer(m_Device.GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device.GetDevice(), stagingBufferMemory, nullptr);
	}

	void Model::CreateIndexBuffers(const std::vector<uint32_t>& indices)
	{
		m_IndexCount = static_cast<uint32_t>(indices.size());
		m_HasIndexBuffer = m_IndexCount > 0;

		if (!m_HasIndexBuffer) return;

		//Create a buffer on the GPU and allocate memory for it
		const VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;

		VkBuffer stagingBuffer{};
		VkDeviceMemory stagingBufferMemory{};
		m_Device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		//Create a region on the CPU which is linked to the GPU buffer we created before
		vkMapMemory(m_Device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data); //Copy the indices from the CPU to the GPU
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_Device.GetDevice(), stagingBufferMemory); //Unmap the memory from the CPU

		m_Device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

		m_Device.CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

		vkDestroyBuffer(m_Device.GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device.GetDevice(), stagingBufferMemory, nullptr);
	}

	void Model::Bind(VkCommandBuffer commandBuffer) const
	{
		const VkBuffer buffers[] = { m_VertexBuffer };
		constexpr VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (m_HasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
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
