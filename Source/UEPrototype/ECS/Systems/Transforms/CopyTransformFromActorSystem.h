#pragma once

#include "Components/GeneralComponents.h"
#include "Systems/SystemBase.h"
#include "Components/TransformComponents.h"


class  CopyTransformFromActorSystem final :public SystemBase<CopyTransformFromActor, LocalToWorld, FActorComponent>
{
public:
    void OnCreate() override;
};
