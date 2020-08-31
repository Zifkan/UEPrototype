#pragma once


#include "ECSPlugin/ECS/Components/GeneralComponents.h"
#include "ECSPlugin/ECS/Components/TransformComponents.h"
#include "ECSPlugin/ECS/Systems/SystemBase.h"

class CopyTransformToActorSystem final :public SystemBase< LocalToWorld, FActorComponent>
{
public:
    void OnCreate() override;
};