#pragma once

#include "Components/GeneralComponents.h"
#include "Systems/SystemBase.h"
#include "Components/TransformComponents.h"


class  CopyTransformFromActorSystem final :public SystemBase<LocalToWorld, FActorComponent>
{
public:
    CopyTransformFromActorSystem() : SystemBase("CopyTransformFromActorSystem"){}    

    void OnCreate() override;
};
