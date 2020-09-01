#pragma once

#include "Components/GeneralComponents.h"
#include "Components/TransformComponents.h"
#include "Systems/SystemBase.h"

class CopyTransformToActorSystem final :public SystemBase< LocalToWorld, FActorComponent>
{
public:
    void OnCreate() override;
};