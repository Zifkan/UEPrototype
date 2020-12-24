#pragma once
#include "CheckActionAvailableSystem.h"
#include "MovementStateSystem.h"
#include "Core/EcsWorld.h"
#include "Systems/SystemLauncher.h"


class CharacterStateMachineSystemLaunch: public SystemLauncher
{
public: 

    CharacterStateMachineSystemLaunch(FEcsWorld* world) : SystemLauncher(world)
    {
        RegisterSystem(new CheckActionAvailableSystem());
        RegisterSystem(new MovementStateSystem());
        RegisterSystem(new AttackStateSystem());       
     
    }    
};
