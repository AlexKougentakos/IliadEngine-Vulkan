#pragma once

#include <vector>
#include <memory>
#include "GameObject.h"

namespace ili 
{
    class Scene 
    {
    public:
        virtual ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        GameObject* CreateGameObject();
        void RemoveGameObject(unsigned int id);
        std::vector<std::unique_ptr<GameObject>>& GetGameObjects();

        virtual void Initialize() = 0;
        
	protected:
        friend class SceneManager; //Factory pattern
        Scene() = default;

        virtual void Update(float deltaTime) = 0;

        std::vector<std::unique_ptr<GameObject>> m_pGameObjects;
		unsigned int m_IdCounter{};
    };
}
