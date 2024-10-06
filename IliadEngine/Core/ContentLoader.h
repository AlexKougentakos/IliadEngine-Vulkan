#pragma once

#include "Singleton.h"
#include "Graphics/Model.h"
#include "Graphics/Device.h"
#include <memory>
#include <string>

#include "Graphics/Texture.h"

namespace ili
{

    class ContentLoader final : public Singleton<ContentLoader>
    {
    public:
        ContentLoader(const ContentLoader& other) = delete;
        ContentLoader(ContentLoader&& other) noexcept = delete;
        ContentLoader& operator=(const ContentLoader& other) = delete;
        ContentLoader& operator=(ContentLoader&& other) noexcept = delete;
        virtual ~ContentLoader() override = default;

        void Initialize(ili::Device* device)
        {
            m_pDevice = device;
        }

        std::shared_ptr<Model> LoadModelFromFile(const std::string& filepath) const;
        std::shared_ptr<Texture> LoadTextureFromFile(const std::string& filepath) const;
        std::shared_ptr<Texture> CreateTextureFromColor(const glm::vec4& color);
    private:
        friend class Singleton<ContentLoader>;
        ContentLoader() = default;

        ili::Device* m_pDevice = nullptr;
        std::shared_ptr<Texture> LoadTextureFromData(VkExtent3D extent, VkFormat format, const void* data, VkDeviceSize dataSize) const;

        struct Builder
        {
            std::vector<ili::Model::Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void LoadModel(const std::string& filepath);
        };
    };
}
