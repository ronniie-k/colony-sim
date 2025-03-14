#include "World.hpp"

std::shared_ptr<World> World::m_Instance = std::make_shared<World>();

uint32_t World::CreateEntity()
{
    return m_NextEntityId++;
}

std::weak_ptr<World> World::Get()
{
    return m_Instance;
}
