#pragma once

#include <vector>
#include <memory>
#include "GameObject.h"

namespace ili 
{
    class Scene 
    {
    public:
        Scene() = default;
        ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        GameObject* CreateGameObject();
        void RemoveGameObject(unsigned int id);
        std::vector<std::unique_ptr<GameObject>>& GetGameObjects();

        void Update(float deltaTime);
    private:
        std::vector<std::unique_ptr<GameObject>> m_pGameObjects;
		unsigned int m_IdCounter{};
    };
}
