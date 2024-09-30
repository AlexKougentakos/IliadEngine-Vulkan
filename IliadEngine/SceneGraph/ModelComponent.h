#pragma once
#include "./Graphics/Model.h"
#include "BaseComponent.h"
#include <memory>
#include <string>

namespace ili
{

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

	private:
		std::shared_ptr<Model> m_pModel{};

	};
}
