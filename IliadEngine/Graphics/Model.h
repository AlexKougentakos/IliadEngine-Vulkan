#pragma once

#include "Graphics/Device.h"
#include "Graphics/Buffer.h"
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>
#include <memory>

namespace ili
{
    class Model
    {
    public:
        struct Vertex
        {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 texCoord{};

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

            bool operator==(const Vertex& other) const
            {
                return position == other.position && color == other.color && normal == other.normal && texCoord == other.texCoord;
            }
        };

        Model(Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~Model();

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        void Bind(VkCommandBuffer commandBuffer) const;
        void Draw(VkCommandBuffer commandBuffer) const;
    private:
        void CreateVertexBuffers(const std::vector<Vertex>& vertices);
        void CreateIndexBuffers(const std::vector<uint32_t>& indices);

        Device& m_Device;

        std::unique_ptr<Buffer> m_pVertexBuffer;
        uint32_t m_VertexCount;

        bool m_HasIndexBuffer = false;
        std::unique_ptr<Buffer> m_pIndexBuffer;
        uint32_t m_IndexCount;
    };
}
