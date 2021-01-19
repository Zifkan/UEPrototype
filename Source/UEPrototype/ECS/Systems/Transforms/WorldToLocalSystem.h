#pragma once

#include "Components/TransformComponents.h"
#include "Systems/SystemBase.h"


class WorldToLocalSystem final :public SystemBase<LocalToWorld, WorldToLocal>
{
public:
    WorldToLocalSystem() : SystemBase("WorldToLocalSystem"){}    

    void OnCreate() override;
};