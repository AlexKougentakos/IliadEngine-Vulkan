#pragma once
#include "./Graphics/Model.h"
#include "BaseComponent.h"
#include <memory>
#include <string>

namespace ili
{
    class Texture;

    class ModelComponent final : public BaseComponent
    {
    public:
        ModelComponent(const std::shared_ptr<Model>& pModel) : m_pModel(pModel) {}
        ModelComponent(const std::string& modelPath);
        ModelComponent() = default;
        virtual ~ModelComponent() override = default;

        ModelComponent(const ModelComponent&) = delete;
        ModelComponent(ModelComponent&&) = delete;
        ModelComponent& operator=(const ModelComponent&) = delete;
        ModelComponent& operator=(ModelComponent&&) = delete;

        virtual void Initialize() override;

        void SetModel(const std::shared_ptr<Model>& pModel) { m_pModel = pModel; }
        std::shared_ptr<Model> GetModel() const { return m_pModel; }

        // Existing diffuse map functions
        void SetDiffuseMap(const std::shared_ptr<Texture>& pDiffuseMap) { m_DiffuseMap = pDiffuseMap; }
        std::shared_ptr<Texture> GetDiffuseMap() const { return m_DiffuseMap; }

        // New functions for PBR textures
        void SetNormalMap(const std::shared_ptr<Texture>& pNormalMap) { m_NormalMap = pNormalMap; }
        std::shared_ptr<Texture> GetNormalMap() const { return m_NormalMap; }

        void SetMetallicMap(const std::shared_ptr<Texture>& pMetallicMap) { m_MetallicMap = pMetallicMap; }
        std::shared_ptr<Texture> GetMetallicMap() const { return m_MetallicMap; }

        void SetRoughnessMap(const std::shared_ptr<Texture>& pRoughnessMap) { m_RoughnessMap = pRoughnessMap; }
        std::shared_ptr<Texture> GetRoughnessMap() const { return m_RoughnessMap; }

        void SetAOMap(const std::shared_ptr<Texture>& pAOMap) { m_AOMap = pAOMap; }
        std::shared_ptr<Texture> GetAOMap() const { return m_AOMap; }

    private:
        std::shared_ptr<Model> m_pModel{};

        std::shared_ptr<Texture> m_DiffuseMap{};
        std::shared_ptr<Texture> m_NormalMap{};
        std::shared_ptr<Texture> m_MetallicMap{};
        std::shared_ptr<Texture> m_RoughnessMap{};
        std::shared_ptr<Texture> m_AOMap{};
    };
}
