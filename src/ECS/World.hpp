#pragma once

#include <array>
#include <memory>
#include <unordered_map>

#include "Component.hpp"

class World
{
public:    
    uint32_t CreateEntity();
    template<class T, class... Args>
    std::weak_ptr<Component> AddComponent(uint32_t entityId, Args&&... args);
    
    static std::weak_ptr<World> Get();
    
private:
    std::unordered_map<size_t, std::vector<std::shared_ptr<Component>>> m_Components;
    
    uint32_t m_NextEntityId = 0;
    
    static std::shared_ptr<World> m_Instance;
};

template <class T, class... Args>
std::weak_ptr<Component> World::AddComponent(uint32_t entityId, Args&&... args)
{
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    static_assert(!std::is_same_v<T, Component>, "T must not be Component");

    auto& components = m_Components[typeid(T).hash_code()];
    components.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));

    return components.back();
}