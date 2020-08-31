#pragma once

#include <vector>
#include "SystemBase.h"

class SystemLauncher
{

public:
    SystemLauncher(FEcsWorld* world) 
    {
        instances.reserve(128);
        m_pWorld = world;
    }

    ~SystemLauncher()
    {
        release();
    }

    bool RegisterSystem(IBaseSystem* s)
    {
        s->Init(m_pWorld);
        s->OnCreate();
        instances.push_back(s);
        return true;
    }

    void release()
    {
        for (auto el : instances)
        {
            delete el;
        }
    }

    std::vector<IBaseSystem*>& getInstances()
    {
        return instances;
    }

     void Update(float delta_time = 0.0f, void *param = nullptr)
    {        
        for (auto el : instances)
        {
            el->OnUpdate(delta_time, param);
        }       
    }

private:
    std::vector<IBaseSystem*> instances;
    FEcsWorld* m_pWorld;
};
