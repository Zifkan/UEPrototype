#pragma once
#include "RendererGroup.h"
#include "Components/RendererRoot.h"
#include "Systems/SystemBase.h"

class  BoneMatrixCalculationSystem final :public SystemBase<RendererRoot>
{
public:
    virtual void OnCreate() override;

private:    
    RendererGroup* rendererGroup;
};
