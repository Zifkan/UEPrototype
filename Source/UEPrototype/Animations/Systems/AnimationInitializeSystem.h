#pragma once
#include "RendererGroup.h"
#include "Components/RendererRoot.h"
#include "Systems/SystemBase.h"

struct RendererGroupData
{
    int RootRenderId;
    RendererGroupData(int rootRenderId)
    {
        RootRenderId = rootRenderId;
    }
};

class AnimationInitializeSystem :public SystemBase<RendererRoot>
{
public:
    virtual void OnCreate() override;
    RendererGroupData RegisterRendererInGroup(Entity customRendererEntity) const;
    RendererGroup* rendererGroup;
};
