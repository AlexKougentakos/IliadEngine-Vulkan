#pragma once

#include "TransformComponent.h"
#include "../Graphics/Model.h"
#include "glm/ext/matrix_transform.hpp"

namespace ili
{
	class GameObject
	{
	public:
		virtual ~GameObject() = default;

		// Enable move semantics
		GameObject(GameObject&&) noexcept = default;
		GameObject& operator=(GameObject&&) noexcept = default;

		// Delete copy semantics
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;

		void SetModel(std::shared_ptr<Model> model) { m_Model = model; }
		std::shared_ptr<Model> GetModel() const { return m_Model; }

		//void SetColor(const glm::vec3& color) { m_Color = color; }
		//glm::vec3 GetColor() const { return m_Color; }
		
		TransformComponent* GetTransform() const { return m_pTransformComponent; }

		//static GameObject MakePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f, 1.f, 1.f));

		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<BaseComponent, T>::value, "T must derive from BaseComponent");
			auto component = std::make_unique<T>(std::forward<Args>(args)...);
			auto ptr = component.get();
			ptr->m_pGameObject = this;
			m_pComponents.push_back(std::move(component));
			return ptr;
		}

		unsigned int GetId() const { return m_Id; }

	protected:
		virtual void Update() {}
		GameObject(const unsigned int id);
	private:
		friend class Scene;

		void RootUpdate();


		//glm::vec3 m_Color{};
		TransformComponent* m_pTransformComponent{};

		std::vector<std::unique_ptr<BaseComponent>> m_pComponents{};

		std::shared_ptr<Model> m_Model{};

		unsigned int m_Id{};
	};
}
