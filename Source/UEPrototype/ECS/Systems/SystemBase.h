#pragma once

#include "Core/EcsWorld.h"

class IBaseSystem
{
public:   
  
    virtual void Init(FEcsWorld* world) = 0;
    virtual void Init(FEcsWorld* world, UWorld* uWorld) = 0;

    virtual ~IBaseSystem() = default;

    virtual void OnCreate() = 0;
    virtual void OnUpdate(float delta_time = 0.0f, void *param = nullptr) = 0;

    FEcsWorld* GetECSWorld() const { return m_pWorld; }

protected:
    FEcsWorld* m_pWorld = nullptr;
};

template<class ... ComponentType>
class SystemBase : public IBaseSystem
{
public:

    SystemBase(const char *name = nullptr, const char *signature = nullptr)
    {
        this->name = name;
        this->signature = signature;
        deltaTime = 0.0f;
        SystemRun = nullptr;
    }


    void Init(FEcsWorld* world) override
    {
        m_pWorld = world;
        EntityManager = m_pWorld->EntityManager;
        Filter = flecs::filter(*m_pWorld->DefaultWorld);

        SystemRun = new flecs::system<ComponentType...>(*m_pWorld->DefaultWorld, name,signature);
    }


    virtual void Init(FEcsWorld* world,UWorld* uWorld) override
    {
        m_pWorld = world;
        EntityManager = m_pWorld->EntityManager;
        Filter = flecs::filter(*m_pWorld->DefaultWorld);
        UWorld = uWorld;
        SystemRun = new flecs::system<ComponentType...>(*m_pWorld->DefaultWorld, name, signature);
    }


    virtual ~SystemBase(){};     
   

    void OnUpdate(float delta_time = 0.0f, void *param = nullptr) override
    {        
        SystemRun->run(delta_time, param);
        deltaTime = delta_time;
    }

    float GetDeltaTime() const { return deltaTime; }
  
    FEntityManager* GetEntityManager() const { return  m_pWorld->EntityManager; }
    flecs::filter GetFilter() const { return  Filter; }

    void OnCreate() override = 0;

    flecs::system<ComponentType...>* SystemRun;
    UWorld* GetUWorld() const { return UWorld; }

protected:
    const char *name = nullptr;
    const char *signature = nullptr;

private:
    flecs::filter Filter;
   
    FEntityManager* EntityManager = nullptr;
    float deltaTime;

  

    UWorld* UWorld = nullptr;
};