#include "Entity.hpp"

#include "World.hpp"

Entity::Entity()
{
    auto world = World::Get();
    if (auto ptr = world.lock())
    {
        id = ptr->CreateEntity();
    }
}
