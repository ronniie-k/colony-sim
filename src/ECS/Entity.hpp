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

    template <class T>
    std::weak_ptr<T> GetComponent();
    template <class T>
    bool HasComponent();
    template <class T>
    bool TryGetComponent(std::weak_ptr<T>& component);
    template <class T>
    std::vector<std::weak_ptr<T>> GetComponents();
    
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

template<class T>
std::weak_ptr<T> Entity::GetComponent()
{
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    static_assert(!std::is_same_v<T, Component>, "T must not be Component");

    for (const auto& component : m_Components)
    {
        if (auto ptr = component.lock())
        {
            Logging::Debug("Entity::GetComponent() - {}", typeid(ptr).name());
            if (auto casted = std::dynamic_pointer_cast<T>(ptr)) return casted;
        }
    }
    
    return {};
}

template<class T>
bool Entity::HasComponent()
{
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    static_assert(!std::is_same_v<T, Component>, "T must not be Component");

    for (const auto& component : m_Components)
    {
        if (auto ptr = component.lock())
        {
            if (auto casted = std::dynamic_pointer_cast<T>(ptr)) return true;
        }
    }

    return false;
}

template <class T>
bool Entity::TryGetComponent(std::weak_ptr<T>& outComponent)
{
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    static_assert(!std::is_same_v<T, Component>, "T must not be Component");

    for (const auto& component : m_Components)
    {
        if (auto ptr = component.lock())
        {
            if (auto casted = std::dynamic_pointer_cast<T>(ptr))
            {
                outComponent = casted;
                return true;
            }
        }
    }

    outComponent.reset();    
    return false;
}

template<class T>
std::vector<std::weak_ptr<T>> Entity::GetComponents()
{
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    static_assert(!std::is_same_v<T, Component>, "T must not be Component");

    std::vector<std::weak_ptr<T>> components;
    for (const auto& component : m_Components)
    {
        if (auto ptr = component.lock())
        {
            if (auto casted = std::dynamic_pointer_cast<T>(ptr)) components.emplace_back(casted);
        }
    }

    return components;
}