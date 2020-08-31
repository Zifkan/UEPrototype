#pragma once

#include "ECSPlugin/ECS/Components/TransformComponents.h"
#include "ECSPlugin/ECS/Systems/SystemBase.h"


class WorldToLocalSystem final :public SystemBase<LocalToWorld, WorldToLocal>
{
public:
    void OnCreate() override;
};