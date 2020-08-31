#pragma once

#include "flecs.h"

class FEntityManager
{
public:
        FEntityManager(flecs::world *world)
        {
            defaultWorld = world;

        };
        
        virtual ~FEntityManager()
        {
            
        }

        template <typename T>
        void RegisterComponent(const char *name = nullptr)
        {
            flecs::component<T>(*defaultWorld, name);
        }

        ///TODO Double copy here. Thinking about later
        flecs::entity CreateEntity() const
        {
            return  flecs::entity(*defaultWorld);
        }

        flecs::entity CreateEntity(flecs::entity_t id) const
        {
            return  flecs::entity(*defaultWorld,id);
        }

        flecs::entity CreateEntity(const char* name) const
        {
            return  flecs::entity(*defaultWorld, name);
        }

        template <typename T>
        flecs::entity CreateEntityWithComponent(const T&& value) const
        {
            return flecs::entity(*defaultWorld).set<T>(value);
        }

        template <typename T>
        void SetComponentData(flecs::entity e, const T& value) const
        {
            e.set<T>(value);
        }

        template <typename T>
        void AddComponentData(flecs::entity e) const
        {
            e.add<T>();
            
        }

        void RemoveEntity(flecs::entity e) const
        {
            e.remove(e);
        }

        bool HasComponent(flecs::entity e) const
        {          
            return e.owns(e);
        }

        template <typename T>
        T* RemoveComponent(flecs::entity e)
        {            
            return e.remove<T>();
        }

       
        flecs::type SetType(const char* tagName, const char *expr = nullptr)
        {          
            return flecs::type(*defaultWorld,tagName, expr);
        }
                     

private:
        flecs::world* defaultWorld;
};
