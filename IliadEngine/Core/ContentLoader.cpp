#include "ContentLoader.h"
#include "../Core/Utils.h"
#include <stdexcept>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>
#include <tiny_obj_loader.h>

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
    void ContentLoader::Builder::LoadModel(const std::string& filepath)
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

        std::unordered_map<ili::Model::Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                ili::Model::Vertex vertex{};

                if (index.vertex_index >= 0)
                {
                    vertex.position =
                    {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    if (attrib.colors.size() >= 3 * (index.vertex_index + 1))
                    {
                        vertex.color =
                        {
                            attrib.colors[3 * index.vertex_index + 0],
                            attrib.colors[3 * index.vertex_index + 1],
                            attrib.colors[3 * index.vertex_index + 2]
                        };
                    }
                    else
                    {
                        vertex.color = { 1.0f, 1.0f, 1.0f };
                    }
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
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
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

    std::shared_ptr<ili::Model> ContentLoader::LoadModelFromFile(const std::string& filepath) const
    {
        assert(m_pDevice != nullptr && "Device is not initialized");

        Builder builder{};
        builder.LoadModel(filepath);

        return std::make_shared<ili::Model>(*m_pDevice, builder.vertices, builder.indices);
    }

    std::shared_ptr<Texture> ContentLoader::LoadTextureFromFile(const std::string& filepath) const
    {
		assert(m_pDevice != nullptr && "Device is not initialized");

		return std::make_shared<Texture>(*m_pDevice, filepath);
    }

    std::shared_ptr<Texture> ContentLoader::CreateTextureFromColor(const glm::vec4& color)
    {
        // Create a 1x1 pixel with the specified color
        VkExtent3D extent = { 1, 1, 1 };
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB; // Changed to sRGB format

        uint8_t pixelData[4];
        // Ensure color components are within [0, 1] before scaling
        pixelData[0] = static_cast<uint8_t>(glm::clamp(color.r, 0.0f, 1.0f) * 255);
        pixelData[1] = static_cast<uint8_t>(glm::clamp(color.g, 0.0f, 1.0f) * 255);
        pixelData[2] = static_cast<uint8_t>(glm::clamp(color.b, 0.0f, 1.0f) * 255);
        pixelData[3] = static_cast<uint8_t>(glm::clamp(color.a, 0.0f, 1.0f) * 255);

        return LoadTextureFromData(extent, format, pixelData, sizeof(pixelData));
    }

    std::shared_ptr<Texture> ContentLoader::LoadTextureFromData(VkExtent3D extent, VkFormat format, const void* data, VkDeviceSize dataSize) const
    {
        assert(m_pDevice != nullptr && "Device is not initialized");

        auto texture = std::make_unique<Texture>(
            *m_pDevice,
            format,
            extent,
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            VK_SAMPLE_COUNT_1_BIT);

        // Upload data to the texture
        texture->UploadData(data, dataSize);

        return texture;
    }
}