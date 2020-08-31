#pragma once

#include "ECSPlugin/ECS/Systems/SystemBase.h"
#include "ECSPlugin/ECS/Components/TransformComponents.h"


class CopyTransformFromActorSystem final :public SystemBase
{
public:
    void OnCreate() override;
};
