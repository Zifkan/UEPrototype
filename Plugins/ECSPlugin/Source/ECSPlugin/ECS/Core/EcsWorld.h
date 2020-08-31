#pragma once

#include "EntityManager.h"

class FEcsWorld
{
public:

    FEcsWorld()
    {
        DefaultWorld = new flecs::world();
        EntityManager = new FEntityManager(DefaultWorld);
    }

    virtual ~FEcsWorld()
    {
        ecs_fini(DefaultWorld->c_ptr());

        delete DefaultWorld;
        delete EntityManager;
    }

   

    FEntityManager* EntityManager;
    flecs::world* DefaultWorld;


    static FEcsWorld* instance()
    {
               static FEcsWorld* _instance = nullptr;

        if (_instance == nullptr)
        {
            _instance = new FEcsWorld();
        }
        return _instance;
    }


};

