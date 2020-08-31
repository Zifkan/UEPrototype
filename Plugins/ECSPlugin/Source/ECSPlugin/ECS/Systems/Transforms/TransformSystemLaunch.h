#pragma once

#include "CopyTransformFromActorSystem.h"
#include "CopyTransformToActorSystem.h"
#include "TRSToLocalToWorldSystem.h"
#include "WorldToLocalSystem.h"
#include "ECSPlugin/ECS/Systems/SystemLauncher.h"



class TransformSystemLaunch : public SystemLauncher
{
public:
    explicit TransformSystemLaunch(FEcsWorld* world)
        : SystemLauncher(world)
    {
        RegisterSystem(new CopyTransformFromActorSystem());
        RegisterSystem(new TRSToLocalToWorldSystem());
        RegisterSystem(new WorldToLocalSystem());
        RegisterSystem(new CopyTransformToActorSystem());

    }


};
