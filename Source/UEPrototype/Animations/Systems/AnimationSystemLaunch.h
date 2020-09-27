#pragma once
#include "AnimationInitializeSystem.h"
#include "BoneMatrixCalculationSystem.h"
#include "Systems/SystemLauncher.h"

class AnimationSystemLaunch: public SystemLauncher
{
public:
    AnimationSystemLaunch(FEcsWorld* world) : SystemLauncher(world)
    {
        RegisterSystem(new AnimationInitializeSystem());
        RegisterSystem(new BoneMatrixCalculationSystem());
    }
};
