#pragma once
#include "Components/RendererRoot.h"
#include "Systems/SystemBase.h"

class  BoneMatrixCalculationSystem final :public SystemBase<RendererRoot>
{
    public:
    virtual void OnCreate() override;
};
