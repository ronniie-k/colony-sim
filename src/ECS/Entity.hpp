#pragma once
#include <cstdint>
#include <print>

#include <type_traits>

#include "Component.hpp"
#include "World.hpp"
#include "Utils/Logging.hpp"

struct Entity
{
    Entity();
    
    const uint32_t Id() const { return id; }

    template <class T, class... Args>
    void AddComponent(Args&&... args);
    
private:
    uint32_t id;
    std::vector<std::weak_ptr<Component>> m_Components;
};

template <class T, class... Args>
void Entity::AddComponent(Args&&... args)
{
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    static_assert(!std::is_same_v<T, Component>, "T must not be Component");
    
    if (auto world = World::Get().lock())
    {
        m_Components.emplace_back(world->AddComponent<T>(id, std::forward<Args>(args)...));
    }
}
