#pragma once



#include "flecs.h"

using Entity = flecs::entity;

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
        void RegisterComponent(const char *name = nullptr) const
        {
            flecs::component<T>(*defaultWorld, name);
        }

        ///TODO Double copy here. Thinking about later
        flecs::entity CreateEntity() const
        {
            return  flecs::entity(*defaultWorld);
        }

        Entity CreateEntity(flecs::entity_t id) const
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

        template <typename T>
        void AddComponentDataSafety(flecs::entity e) const
        {
            if(!e.has<T>())
            {
                e.add<T>();
            }
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

        template <typename T>
        const T* GetComponent(flecs::entity e)
        {
            return e.get<T>();
        }


        Entity GetFirstParent(flecs::entity e)
        {

            for(auto cid : e.type().vector())
            {
                if (cid & flecs::Childof)
                {
                    return flecs::entity(*defaultWorld, cid & ECS_COMPONENT_MASK);
                }
            }

            return e.null();
        }
       
        
        flecs::world* defaultWorld;      
};
