#pragma once

#include "CopyTransformFromActorSystem.h"
#include "CopyTransformToActorSystem.h"
#include "TRSToLocalToWorldSystem.h"
#include "WorldToLocalSystem.h"
#include "Systems/SystemLauncher.h"



class TransformSystemLaunch : public SystemLauncher
{
public:
    TransformSystemLaunch(FEcsWorld* world) : SystemLauncher(world)
    {
        RegisterSystem(new CopyTransformFromActorSystem());
        RegisterSystem(new TRSToLocalToWorldSystem());
        RegisterSystem(new WorldToLocalSystem());
        RegisterSystem(new CopyTransformToActorSystem());

    }


};
