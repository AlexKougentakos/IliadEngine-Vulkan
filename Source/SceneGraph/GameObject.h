#pragma once

#include "../Graphics/Model.h"

namespace ili
{

	struct Transform2DComponent
	{
		glm::vec2 position{};

		glm::mat2 GetMatrix() const { return glm::mat2{ 1.f }; }; //Identity matrix
	};

	class GameObject
	{
	public:

		~GameObject() = default;

		GameObject(const GameObject&) = default;
		GameObject(GameObject&&) = default;
		GameObject& operator=(const GameObject&) = delete;
		GameObject& operator=(GameObject&&) = delete;


		//Todo:: move this to the Scene when I add it
		static GameObject Create() 
		{ 
			static unsigned int currentId = 0;
			return GameObject(currentId++);
		}

		void SetModel(std::shared_ptr<Model> model) { m_Model = model; }
		std::shared_ptr<Model> GetModel() const { return m_Model; }

		void SetColor(const glm::vec3& color) { m_Color = color; }
		glm::vec3 GetColor() const { return m_Color; }
		
		Transform2DComponent& GetTransform() { return m_Transform2D; }
		const Transform2DComponent& GetTransformConst() const { return m_Transform2D; }


		void Translate(const glm::vec2& translation) { m_Transform2D.position += translation; }

		unsigned int GetId() const { return m_Id; }
	private:
		GameObject(const unsigned int id) : m_Id(id) {}

		std::shared_ptr<Model> m_Model{};
		glm::vec3 m_Color{};
		Transform2DComponent m_Transform2D{};

		unsigned int m_Id{};
	};
}