#pragma once

#include <vector>
#include <memory>
#include "GameObject.h"
#include "../Core/PointLight.h"

namespace ili 
{
	class Scene 
    {
    public:
        virtual ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        // Templated function to create and add a GameObject or its derived type
        template <typename T, typename... Args>
        T* CreateGameObject(Args&&... args) 
        {
            // Ensure T is derived from GameObject
            //static_assert(std::is_base_of<GameObject, T>::value, "T must inherit from GameObject");

            // Create a raw pointer to the new object
            // Pass m_IdCounter++ as the first argument to the constructor
            T* obj = new T(m_IdCounter++, std::forward<Args>(args)...);

            // Wrap the raw pointer in a unique_ptr and store it
            m_pGameObjects.push_back(std::unique_ptr<GameObject>(obj));

            // Return the raw pointer to the created object
            return obj;
        }

        PointLightGameObject* CreatePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f, 1.f, 1.f));

        void RemoveGameObject(unsigned int id);
        std::vector<std::unique_ptr<GameObject>>& GetGameObjects() { return m_pGameObjects; }
		std::vector<std::unique_ptr<PointLightGameObject>>& GetPointLights() { return m_pPointLights; }


        virtual void Initialize() = 0;
        
	protected:
        friend class SceneManager; //Factory pattern
        Scene() = default;

        virtual void Update(float deltaTime) = 0;

        std::vector<std::unique_ptr<GameObject>> m_pGameObjects{};
        std::vector<std::unique_ptr<PointLightGameObject>> m_pPointLights{};
		unsigned int m_IdCounter{};
    };
}
