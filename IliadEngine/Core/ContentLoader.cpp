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
}
