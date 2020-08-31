#pragma once

#include "ECSPlugin/ECS/Components/GeneralComponents.h"
#include "ECSPlugin/ECS/Systems/SystemBase.h"
#include "ECSPlugin/ECS/Components/TransformComponents.h"


class CopyTransformFromActorSystem final :public SystemBase<CopyTransformFromActor, LocalToWorld, FActorComponent>
{
public:
    void OnCreate() override;
};
